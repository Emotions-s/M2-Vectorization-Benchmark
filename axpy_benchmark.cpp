#include <iostream>
#include <chrono>
#include <arm_neon.h>

int main()
{
    const int N_START = 1000;
    const int N_STOP = 2 << 28;
    const double N_RUN = 100.0;

    for (int N = N_START; N <= N_STOP; N *= 2)
    {
        int a = rand() % 10;
        int32x4_t a_vector = vdupq_n_s32(a);
        int *b = new int[N];
        int *c_scalar = new int[N];
        int *c_vector = new int[N];
        int *c_scalar_unroll4 = new int[N];
        int *c_vector_unroll4 = new int[N];

        for (int i = 0; i < N; i++)
        {
            b[i] = rand() % 10;
        }

        double vector_time = 0.0, scalar_time = 0.0, scalar_unroll4_time = 0.0, vector_unroll4_time = 0.0;

        for (int m = 0; m < N_RUN; m++)
        {
            for (int i = 0; i < N; i++)
            {
                c_scalar[i] = 0;
                c_vector[i] = 0;
                c_scalar_unroll4[i] = 0;
                c_vector_unroll4[i] = 0;
            }

            std::chrono::time_point<std::chrono::high_resolution_clock> start, end;

            start = std::chrono::high_resolution_clock::now();

            for (int i = 0; i < N; i++)
            {
                c_scalar[i] += a * b[i];
            }

            end = std::chrono::high_resolution_clock::now();
            scalar_time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

            start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < N; i += 4)
            {
                c_scalar_unroll4[i] += a * b[i];
                c_scalar_unroll4[i + 1] += a * b[i + 1];
                c_scalar_unroll4[i + 2] += a * b[i + 2];
                c_scalar_unroll4[i + 3] += a * b[i + 3];
            }

            end = std::chrono::high_resolution_clock::now();
            scalar_unroll4_time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

            start = std::chrono::high_resolution_clock::now();

            for (int i = 0; i < N; i += 4)
            {
                vst1q_s32(&c_vector[i], vmlaq_s32(vld1q_s32(&c_vector[i]), a_vector, vld1q_s32(&b[i])));
            }

            end = std::chrono::high_resolution_clock::now();
            vector_time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

            start = std::chrono::high_resolution_clock::now();

            for (int i = 0; i < N; i += 16)
            {
                vst1q_s32(&c_vector_unroll4[i], vmlaq_s32(vld1q_s32(&c_vector_unroll4[i]), a_vector, vld1q_s32(&b[i])));
                vst1q_s32(&c_vector_unroll4[i + 4], vmlaq_s32(vld1q_s32(&c_vector_unroll4[i + 4]), a_vector, vld1q_s32(&b[i + 4])));
                vst1q_s32(&c_vector_unroll4[i + 8], vmlaq_s32(vld1q_s32(&c_vector_unroll4[i + 8]), a_vector, vld1q_s32(&b[i + 8])));
                vst1q_s32(&c_vector_unroll4[i + 12], vmlaq_s32(vld1q_s32(&c_vector_unroll4[i + 12]), a_vector, vld1q_s32(&b[i + 12])));
            }

            end = std::chrono::high_resolution_clock::now();
            vector_unroll4_time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

            if (m == 0)
            {
                for (int i = 0; i < N; i++)
                {
                    if (c_scalar[i] != c_vector[i] || c_scalar[i] != c_scalar_unroll4[i] || c_scalar[i] != c_vector_unroll4[i])
                    {
                        std::cout << "wrong" << std::endl;
                        return -1;
                    }
                }
            }
        }

        printf("scalar,%d,%f\n", N, scalar_time / N_RUN / N);
        printf("vector,%d,%f\n", N, vector_time / N_RUN / N);
        printf("scalar unroll4,%d,%f\n", N, scalar_unroll4_time / N_RUN / N);
        printf("vector unroll4,%d,%f\n", N, vector_unroll4_time / N_RUN / N);
    }

    return 0;
}
