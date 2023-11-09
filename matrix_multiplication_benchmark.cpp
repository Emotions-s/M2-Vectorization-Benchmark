#include <iostream>
#include <chrono>
#include <arm_neon.h>

int main()
{
    const int N_START = 8;
    const int N_STOP = 2 << 9;
    const double N_RUN = 100.0;;

    for (int N = N_START; N <= N_STOP; N *= 2)
    {
        int **a = new int *[N];
        int **b = new int *[N];
        int **c_scalar = new int *[N];
        int **c_vector = new int *[N];
        int **c_scalar_unroll4 = new int *[N];
        int **c_vector_unroll2 = new int *[N];
        for (int i = 0; i < N; i++)
        {
            a[i] = new int[N];
            b[i] = new int[N];
            c_scalar[i] = new int[N];
            c_vector[i] = new int[N];
            c_scalar_unroll4[i] = new int[N];
            c_vector_unroll2[i] = new int[N];
        }

        double scalar_time = 0.0, vector_time = 0.0, scalar_unroll4_time = 0.0, vector_unroll2_time = 0.0;

        for (int i = 0; i < N; i++)
        {
            for (int j = 0; j < N; j++)
            {
                a[i][j] = rand() % 10;
                b[i][j] = rand() % 10;
            }
        }
        for (int m = 0; m < N_RUN; m++)
        {
            for (int i = 0; i < N; i++)
            {
                for (int j = 0; j < N; j++)
                {
                    c_scalar[i][j] = 0;
                    c_vector[i][j] = 0;
                    c_scalar_unroll4[i][j] = 0;
                    c_vector_unroll2[i][j] = 0;
                }
            }

            std::chrono::time_point<std::chrono::high_resolution_clock> start, end;
            start = std::chrono::high_resolution_clock::now();

            for (int i = 0; i < N; i++)
            {
                for (int k = 0; k < N; k++)
                {
                    for (int j = 0; j < N; j++)
                    {
                        c_scalar[i][j] += a[i][k] * b[k][j];
                    }
                }
            }

            end = std::chrono::high_resolution_clock::now();
            scalar_time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

            start = std::chrono::high_resolution_clock::now();

            for (int i = 0; i < N; i++)
            {
                for (int k = 0; k < N; k++)
                {
                    int32x4_t tmp = vdupq_n_s32(a[i][k]);
                    for (int j = 0; j < N; j += 4)
                    {
                        vst1q_s32(&c_vector[i][j], vmlaq_s32(vld1q_s32(&c_vector[i][j]), tmp, vld1q_s32(&b[k][j])));
                    }
                }
            }

            end = std::chrono::high_resolution_clock::now();
            vector_time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

            start = std::chrono::high_resolution_clock::now();

            for (int i = 0; i < N; i++)
            {
                for (int k = 0; k < N; k++)
                {
                    for (int j = 0; j < N; j += 4)
                    {
                        c_scalar_unroll4[i][j] += a[i][k] * b[k][j];
                        c_scalar_unroll4[i][j + 1] += a[i][k] * b[k][j + 1];
                        c_scalar_unroll4[i][j + 2] += a[i][k] * b[k][j + 2];
                        c_scalar_unroll4[i][j + 3] += a[i][k] * b[k][j + 3];
                    }
                }
            }

            end = std::chrono::high_resolution_clock::now();
            scalar_unroll4_time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

            start = std::chrono::high_resolution_clock::now();

            for (int i = 0; i < N; i++)
            {
                for (int k = 0; k < N; k++)
                {
                    int32x4_t tmp = vdupq_n_s32(a[i][k]);
                    for (int j = 0; j < N; j += 8)
                    {
                        vst1q_s32(&c_vector_unroll2[i][j], vmlaq_s32(vld1q_s32(&c_vector_unroll2[i][j]), tmp, vld1q_s32(&b[k][j])));
                        vst1q_s32(&c_vector_unroll2[i][j + 4], vmlaq_s32(vld1q_s32(&c_vector_unroll2[i][j + 4]), tmp, vld1q_s32(&b[k][j + 4])));
                    }
                }
            }

            end = std::chrono::high_resolution_clock::now();
            vector_unroll2_time += std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

            if (m == 0)
            {
                for (int i = 0; i < N; i++)
                {
                    for (int j = 0; j < N; j++)
                    {
                        if (c_scalar[i][j] != c_vector[i][j] || c_vector[i][j] != c_scalar_unroll4[i][j] || c_scalar_unroll4[i][j] != c_vector_unroll2[i][j])
                        {
                            std::cout << "Wrong" << std::endl;
                            return 1;
                        }
                    }
                }
            }
        }

        for (int i = 0; i < N; i++)
        {
            delete[] a[i];
            delete[] b[i];
            delete[] c_scalar[i];
            delete[] c_vector[i];
            delete[] c_scalar_unroll4[i];
            delete[] c_vector_unroll2[i];
        }
        delete[] a;
        delete[] b;
        delete[] c_scalar;
        delete[] c_vector;
        delete[] c_scalar_unroll4;

        printf("scalar,%d,%f\n", N, scalar_time / N_RUN / (N * N * N));
        printf("vector4,%d,%f\n", N, vector_time / N_RUN / (N * N * N));
        printf("scalar_unroll4,%d,%f\n", N, scalar_unroll4_time / N_RUN / (N * N * N));
        printf("vector4_unroll2,%d,%f\n", N, vector_unroll2_time / N_RUN / (N * N * N));
    }

    return 0;
}
