#include "utils.h"

void load_input_tile_block_from_DRAM (
    fm_t in_fm_buf[IN_BUF_DEPTH][IN_BUF_HEIGHT][IN_BUF_WIDTH], 
    fm_t in_fm[IN_FM_DEPTH][IN_FM_HEIGHT][IN_FM_WIDTH], 
    int  ti, 
    int  tj 
)
{
    const int height_offset = ti * TILE_HEIGHT;  
    const int width_offset  = tj * TILE_WIDTH;

    const int P = PADDING;

    INPUT_BUFFER_DEPTH:
    for(int c = 0; c < IN_BUF_DEPTH; c++)
    {
        INPUT_BUFFER_HEIGHT:
        for (int i = 0; i < IN_BUF_HEIGHT; i++)
        {
        INPUT_BUFFER_WIDTH:
            for (int j = 0; j < IN_BUF_WIDTH; j++)
            {
                // Handle border features here
                if (height_offset + i - P < 0 || width_offset + j - P < 0|| height_offset + i - P > IN_FM_HEIGHT - 1 || width_offset + j - P > IN_FM_WIDTH - 1)
                {
                    in_fm_buf[c][i][j] = 0;
                }
                else
                {
                    in_fm_buf[c][i][j] = in_fm[c][height_offset + i - P][width_offset + j - P];
                }
            }
        }
    }
}

void load_layer_params_from_DRAM (
    wt_t weight_buf[OUT_BUF_DEPTH][IN_BUF_DEPTH][KERNEL_HEIGHT][KERNEL_WIDTH],
    wt_t bias_buf[OUT_BUF_DEPTH],
    wt_t weights[OUT_FM_DEPTH][IN_FM_DEPTH][KERNEL_HEIGHT][KERNEL_WIDTH],
    wt_t bias[OUT_FM_DEPTH],
    int  kernel_group
)
{
    const int kernel_offset  = kernel_group * OUT_BUF_DEPTH;

    WEIGHT_KERNEL_NUM:
    for(int f = 0; f < OUT_BUF_DEPTH; f++)
    {
        WEIGHT_KERNEL_DEPTH:
        for(int c = 0; c < IN_BUF_DEPTH; c++)
        {
            WEIGHT_KERNEL_HEIGHT:
            for(int kh = 0; kh < 7; kh++)
	        {
                WEIGHT_KERNEL_WIDTH:
	            for(int kw = 0; kw < 7; kw++)
	            {
	                weight_buf[f][c][kh][kw] = weights[kernel_offset + f][c][kh][kw];
                }
            }
        }
    }
    
    BIAS:
    for(int f = 0; f < OUT_BUF_DEPTH; f++)
    {
        bias_buf[f] = bias[kernel_offset + f];
    }

}

void store_output_tile_to_DRAM (
    fm_t out_fm[OUT_FM_DEPTH][OUT_FM_HEIGHT][OUT_FM_WIDTH], 
    fm_t out_fm_buf[OUT_BUF_DEPTH][OUT_BUF_HEIGHT][OUT_BUF_WIDTH], 
    int  ti,
    int  tj,
    int  kernel_group
)
{
    const int depth_offset  = kernel_group * OUT_BUF_DEPTH;
    const int height_offset = ti * OUT_BUF_HEIGHT;
    const int width_offset  = tj * OUT_BUF_WIDTH;

    OUTPUT_BUFFER_DEPTH:
    for(int f = 0; f < OUT_BUF_DEPTH; f++)
    {
        OUTPUT_BUFFER_HEIGHT:
        for(int i = 0; i < OUT_BUF_HEIGHT; i++)
        {
            OUTPUT_BUFFER_WIDTH:
            for(int j = 0; j < OUT_BUF_WIDTH; j++)
            {
                out_fm[depth_offset + f][height_offset + i][width_offset + j] = out_fm_buf[f][i][j];
            }
        }
    }
}
