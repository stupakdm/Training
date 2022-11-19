#include <torch/torch.h>
#include <torch/script.h>
#include <torch/extension.h>
#include "mmintrin.h"
#include "x86intrin.h"
#include "emmintrin.h"
#include "stdlib.h"
#include "stdio.h"
#include <vector>
#define CLEAN 0
#define SSE 1
#define AVX 2

float vector_mult_sse(float *x, float *y, int N_size)
{
    __m128 p,s;

    s = _mm_set_ps1(0);
    int i=0;

    for (;i+3<N_size;i+=4)
    {
        p = _mm_mul_ps(_mm_loadu_ps(x+i), _mm_loadu_ps(y+i));
        s = _mm_add_ps(s,p);
    }
    
    float result[4];
    _mm_storeu_ps(result,s);
    float sum = 0;
    sum = result[0]+result[1]+result[2]+result[3];

    //прибавляем осташиеся
    for (; i<N_size;i++)
        sum += x[i]*y[i];

    return sum;

}

float vector_mult_avx(float *x, float *y, int N_size)
{
    __m256 s = _mm256_setzero_ps();
    
    int i=0;

    for (;i+7<N_size;i+=8)
    {
        s = _mm256_fmadd_ps(_mm256_loadu_ps(x + i),_mm256_loadu_ps(y + i),s);
    }
    
    float result[8];
    _mm256_storeu_ps(result,s);
    float sum = result[0];
    for (int j=1;j<8;j++)
        sum+=result[j];
    //sum = result[0]+result[1]+result[2]+result[3];

    // прибавляем осташиеся
    for (; i<N_size;i++)
        sum += x[i]*y[i];

    return sum;

}

at::Tensor clean_multiply(const at::Tensor& input, const at::Tensor& weight)
{
    int input_x = input.sizes()[0];
    int input_y = input.sizes()[1];
    int weight_x = weight.sizes()[0];
    int weight_y = weight.sizes()[1];
    
    
    float inputs[input_x][input_y];
    std::memcpy(inputs, input.data_ptr(), sizeof(float)*input.numel());
    
    float weights[weight_x][weight_y];
    std::memcpy(weights, weight.data_ptr(), sizeof(float)*weight.numel());

        
    float output[input_x][weight_x];
    
    for (int i=0;i<input_x;++i)
    {
        for (int j=0; j<weight_x;++j)
        {
            output[i][j] = 0;
            for (int k=0;k<input_y;++k)
                output[i][j] += inputs[i][k]*weights[j][k];
        }
    }
    
    torch::Tensor torch_tensor = torch::from_blob(output,
                                                  {input_x, weight_x},
                                                  torch::TensorOptions().dtype(torch::kFloat32).device(torch::kCPU).requires_grad(true));
    
    return torch_tensor.clone();

}


at::Tensor multiply_matrix(const at::Tensor& input, const at::Tensor& weight, bool flag=true)
{
    int input_x = input.sizes()[0];
    int input_y = input.sizes()[1];
    int weight_x = weight.sizes()[0];
    int weight_y = weight.sizes()[1];


    float inputs[input_x][input_y];
    std::memcpy(inputs, input.data_ptr(), sizeof(float)*input.numel());
    
    float weights[weight_x][weight_y];
    std::memcpy(weights, weight.data_ptr(), sizeof(float)*weight.numel());

        
    float output[input_x][weight_x];
    
    
    if (flag == SSE)
    {
        for (int i =0;i<input_x;i++)
        {
            for (int j=0;j<weight_x;j++)
            {
                output[i][j] = vector_mult_sse(inputs[i], weights[j], input_y);
            }
        }
    }
    else
    {
        for (int i =0;i<input_x;i++)
        {
            for (int j=0;j<weight_x;j++)
            {
                output[i][j] = vector_mult_avx(inputs[i], weights[j], input_y);
            }
        }
    }
    
    torch::Tensor torch_tensor = torch::from_blob(output,
                                                  {input_x, weight_x},
                                                  torch::TensorOptions().dtype(torch::kFloat32).device(torch::kCPU).requires_grad(true));

    return torch_tensor.clone();
}


at::Tensor linear_forward(const torch::Tensor& input, const torch::Tensor& weight, const torch::Tensor& bias={}, int flag=0)
{
    //auto weights = weight;
    at::Tensor torch_tensor;
    switch (flag)
    {
        case 0:
            torch_tensor = clean_multiply(input, weight);
            break;
        case 1:
            torch_tensor = multiply_matrix(input, weight, SSE);
            break;
        case 2:
            torch_tensor = multiply_matrix(input, weight, AVX);
            break;
        default:
            std::cout << "Incorrect args: 0 - without SSE and AVX, 1 - SSE, 2 - AVX\n";
            exit(EXIT_FAILURE);
            break;
    }
    
    
    if (bias.defined()) {
        torch_tensor.add_(bias);
    }
    return torch_tensor;
}

at::Tensor linear_forward_avx(const torch::Tensor& input, const torch::Tensor& weight, const torch::Tensor& bias={})
{
    //auto weights = weight;
    
    at::Tensor torch_tensor = multiply_matrix(input, weight);

    if (bias.defined()) {
        torch_tensor.add_(bias);
    }
    return torch_tensor;
}


std::vector<torch::Tensor> linear_backward(const torch::Tensor& grad_output, const torch::Tensor& input, const torch::Tensor& weight, const torch::Tensor& bias) {
  auto grad_input = torch::matmul(grad_output, weight);
  auto grad_weight = torch::matmul(grad_output.t(), input);
  auto grad_bias = bias.defined() ? grad_output.sum(0, /*keepdim=*/false) : torch::Tensor{};
  return {
    grad_input,
    grad_weight,
    grad_bias
  };
}

PYBIND11_MODULE(TORCH_EXTENSION_NAME, m) {
  m.def("forward", &linear_forward, "linear_forward");
  m.def("backward", &linear_backward, "linear backward");
}
