# Example: HLS implemented CONV layer

This example provided one complete conv layer implemented in HLS. For the whole large convolutional layer to be offloaded to FPGA, it has to be tiled and quantized.

We provide instructions to show the HLS functions (under C simulation) can (1) match the actual output of the conv layer and (2) pass the synthesis. You can implement/test the depth-wise conv layer and the point-wise conv layer for milestone 2 following the same way.


First download the binary files (input, weight, bias, output ground truth) from [this link](https://drive.google.com/drive/folders/1Gz5cT2jSkLRg94sERSq9DeUUAyurd7sG?usp=sharing) and check whether the HLS conv output (using full precision) can match the ground truth.

```
make clean && make debug && ./csim.out
```

You should expect an extremely small MSE like this:
```shell
...
Output feature[62][0][0]: Expected: 0.134867    Actual: 0.134867
Output feature[63][0][0]: Expected: 0.0727339   Actual: 0.0727339

Output MSE:  6.64592e-15
----------------------------------------
Floating-point Simulation SUCCESSFUL!!!
----------------------------------------
```

Second, run the HLS C simulation to see the effect of quantization:

```
make clean && make hls_sim && ./csim.out
```

You should expect the MSE to be significantly larger than the floating point representation:

```
...
Tiled-convolution simulation complete!
Output MSE:  8.14252e-05
----------------------------------------
Fixed-point Simulation SUCCESSFUL!!!
----------------------------------------
```

Feel free to use `hls_debug` instead of `hls_sim` if you want to get more debug info.

Last, let's make sure the synthesis can pass by running:

```
make clean && make synth
```

Reference output:

```
...
INFO: [RTGEN 206-100] Finished creating RTL model for 'tiled_conv'.
INFO: [HLS 200-111]  Elapsed time: 0.83 seconds; current allocated memory: 154.460 MB.
INFO: [RTMG 210-278] Implementing memory 'tiled_conv_conv_idEe_ram (RAM)' using block RAMs.
INFO: [RTMG 210-278] Implementing memory 'tiled_conv_conv_weOg_ram (RAM)' using block RAMs.
INFO: [RTMG 210-278] Implementing memory 'tiled_conv_conv_ofYi_ram (RAM)' using block RAMs.
INFO: [HLS 200-111] Finished generating all RTL models Time (s): cpu = 00:00:50 ; elapsed = 00:01:05 . Memory (MB): peak = 658.590 ; gain = 194.027 ; free physical = 21793 ; free virtual = 92355
INFO: [SYSC 207-301] Generating SystemC RTL for tiled_conv.
INFO: [VHDL 208-304] Generating VHDL RTL for tiled_conv.
INFO: [VLOG 209-307] Generating Verilog RTL for tiled_conv.
INFO: [HLS 200-112] Total elapsed time: 65.53 seconds; peak allocated memory: 154.460 MB.
INFO: [Common 17-206] Exiting vivado_hls at Sat Mar 14 22:14:14 2020...
```

You should now be able to see the report called `conv_7x7_csynth.rpt` after the synthesis. You are welcome to (1) look into the report generated and change HLS pragmas to increase the resource utilization (2) follow the vivado tutorial of this course to generate bitstream and test it on the actual FPGA board.

