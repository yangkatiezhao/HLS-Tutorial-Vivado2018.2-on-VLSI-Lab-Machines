#include "utils.h"

void conv_7x7 (
    fm_t Y_buf[OUT_BUF_DEPTH][OUT_BUF_HEIGHT][OUT_BUF_WIDTH], 
    fm_t X_buf[IN_BUF_DEPTH][IN_BUF_HEIGHT][IN_BUF_WIDTH],
    wt_t W_buf[OUT_BUF_DEPTH][IN_BUF_DEPTH][KERNEL_HEIGHT][KERNEL_WIDTH],
    wt_t B_buf[OUT_BUF_DEPTH]
)
{
    #pragma HLS inline off
    CONV_7x7_:
    for (int f = 0; f < OUT_BUF_DEPTH; f++)
    {// Filter Size (Output Depth)
        for (int i = 0; i < OUT_BUF_HEIGHT; i++)             // Output Height
        {
            for (int j = 0; j < OUT_BUF_WIDTH; j++)       // Output Width
            {
            	Y_buf[f][i][j] =  B_buf[f];
                for (int c = 0; c < IN_BUF_DEPTH; c++)        // Input Depth
                {
                    for (int kh = 0; kh < 7; kh++)   // Kernel Height
                    {
                        for (int kw = 0; kw < 7; kw++) // Kernel Width
                        {
                            Y_buf[f][i][j] += X_buf[c][i * STRIDE + kh][j * STRIDE + kw] * W_buf[f][c][kh][kw];
                        }
                    }

                }

            }
        }
    }






















}
