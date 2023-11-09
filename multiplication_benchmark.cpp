#include <iostream>
#include <chrono>
#include <arm_neon.h>

int main()
{
    const int N_START = 1000;
    const int N_STOP = 2 << 28;
    const double N_RUN = 100.0;;

    for (int N = N_START; N <= N_STOP; N *= 2)
    {
        int *a = new int[N];
        int *b = new int[N];
        int *c_scalar = new int[N];
        int *c_vec = new int[N];
        int *c_scalar_unroll4 = new int[N];
        int *c_vec_unroll4 = new int[N];

        for (int i = 0; i < N; i++)
        {
            a[i] = rand() % 10;
            b[i] = rand() % 10;
        }

        double vector_time = 0.0, scalar_time = 0.0, scalar_unroll4_time = 0.0, vector_unroll4_time = 0.0;

        for (int m = 0; m < N_RUN; m++)
        {
            for (int i = 0; i < N; i++)
            {
                c_scalar[i] = 0;
                c_vec[i] = 0;
                c_scalar_unroll4[i] = 0;
                c_vec_unroll4[i] = 0;
            }

            std::chrono::time_point<std::chrono::high_resolution_clock> start, end;

            start = std::chrono::high_resolution_clock::now();

            for (int i = 0; i < N; i++)
            {
                c_scalar[i] = a[i] * b[i];
            }

            end = std::chrono::high_resolution_clock::now();
            scalar_time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

            start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < N; i += 4)
            {
                c_scalar_unroll4[i] = a[i] * b[i];
                c_scalar_unroll4[i + 1] = a[i + 1] * b[i + 1];
                c_scalar_unroll4[i + 2] = a[i + 2] * b[i + 2];
                c_scalar_unroll4[i + 3] = a[i + 3] * b[i + 3];
            }

            end = std::chrono::high_resolution_clock::now();
            scalar_unroll4_time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

            start = std::chrono::high_resolution_clock::now();

            for (int i = 0; i < N; i += 4)
            {
                vst1q_s32(&c_vec[i], vmulq_s32(vld1q_s32(&a[i]), vld1q_s32(&b[i])));
            }

            end = std::chrono::high_resolution_clock::now();
            vector_time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

            start = std::chrono::high_resolution_clock::now();

            for (int i = 0; i < N; i += 16)
            {
                vst1q_s32(&c_vec_unroll4[i], vmulq_s32(vld1q_s32(&a[i]), vld1q_s32(&b[i])));
                vst1q_s32(&c_vec_unroll4[i + 4], vmulq_s32(vld1q_s32(&a[i + 4]), vld1q_s32(&b[i + 4])));
                vst1q_s32(&c_vec_unroll4[i + 8], vmulq_s32(vld1q_s32(&a[i + 8]), vld1q_s32(&b[i + 8])));
                vst1q_s32(&c_vec_unroll4[i + 12], vmulq_s32(vld1q_s32(&a[i + 12]), vld1q_s32(&b[i + 12])));
            }

            end = std::chrono::high_resolution_clock::now();
            vector_unroll4_time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

            if (m == 0)
            {
                for (int i = 0; i < N; i++)
                {
                    if (c_scalar[i] != c_vec[i] || c_scalar[i] != c_scalar_unroll4[i] || c_scalar[i] != c_vec_unroll4[i])
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
