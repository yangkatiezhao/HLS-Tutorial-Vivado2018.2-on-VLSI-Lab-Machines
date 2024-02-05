# HLS Tutorial (Vivado 2018.2 on VLSI Lab Machines)
_Acknowledgment: This tutorial is based on the HLS Tutorial by Yihan Jiang & Akshay Kamath._

## Beginning Steps
#### Step 1
Connect to one VLSI lab machine. 
#### Step 2
Source the environment variables to run Vivado_HLS and Vivado.
```
$ source /home/vlsilab/xilinx/vivado_2018.2/Vivado/2018.2/settings64.sh
```
To avoid this ritual every time you login, add these commands to your `~/.my-bashrc` file. This file gets automatically sourced every time you login.
#### Step 3
Install **faketime** due to this [issue](https://support.xilinx.com/s/question/0D52E00006uxy49SAA/vivado-fails-to-export-ips-with-the-error-message-bad-lexical-cast-source-type-value-could-not-be-interpreted-as-target?language=en_US). We need to set up a proxy server to get to off-campus sites (either proxy1.ece.umn.edu or proxy2.ece.umn.edu and port 3128) using your UMN ID.
```
$ export HTTPS_PROXY=http://username:"passcode"@proxy1.ece.umn.edu:3128/
```
Install Miniconda to use pip: 1) Download `Miniconda3 Linux 64-bit` from this [webpage](https://docs.conda.io/projects/miniconda/en/latest/) locally; 2) Upload to your VLSI account; and 3) install using bash. (Katie: I've tried wget, but it did not work even after setting a proxy server. If anyone has a solution, please share.)
```
$ bash ~/miniconda3/miniconda.sh
```
Install faketime using conda from inside bash shell.
```
$ conda install libfaketime
```
#### Step 4
Launch Vivado_HLS GUI by invoking the following command. (Katie: The latest versions of Vivado/Vitis don't require faketime. I'll notify you once Vitis 2022.1 is installed on our VLSI machines.)
```
$ faketime -f "-4y" vivado_hls
```
Vivado HLS GUI should open as shown below:
![image-0](figures/0.png)


## HLS Tutorial using GUI
Let’s design a vector addition module using C/C++ in Vivado HLS. We will first look at working with Vivado HLS in GUI mode and then in CLI mode. 
#### Steps
1. Create a new project and specify the project name `vector_add`.
   ![image-1](figures/1.png)
2. It is not necessary to specify the top function nor the testbench now. Click on `Next` twice.
   ![image-2](figures/2.png)
   ![image-3](figures/3.png)
3. Change part selection to **Ultra96V2** using the part number `xczu3eg-sbva484-1-e` as shown below. This is the FPGA board we will be using for our lab assignments.
   ![image-4](figures/4.png)
   ![image-5](figures/5.png)
   Ultra96-V2 is an Arm-based, AMD Xilinx Zynq UltraScale+ ™ MPSoC development board, supporting the Pynq (Python for Zynq) framework that makes it easier to run host applications on the board using Python language and libraries. 
4. No need to change the `Solution Name` or the `Period`. We will continue with 100 MHz default clock frequency.
   ![image-6](figures/6.png)
5. You should see a window like the one shown below.
   ![image-7](figures/7.png)
6. Now we can design our accelerator in C++ and simulate with Vivado. To do so, start by creating a new source file named `top.c` in your desired folder as the following:
   ![image-8](figures/8.png)
   ```C++
   // top.c
   void top(int a[100], int b[100], int sum[100]) 
   {
      #pragma HLS interface m_axi port=a depth=100 offset=slave bundle = A
      #pragma HLS interface m_axi port=b depth=100 offset=slave bundle = B
      #pragma HLS interface m_axi port=sum depth=100 offset=slave bundle = SUM
        	
      #pragma HLS interface s_axilite register port=return
        	
      for (int i = 0; i < 100; i++) 
      {
         sum[i] = a[i] + b[i];
      }
   }
   ```
7. Next, create a testbench named `main.c` as the following:
   ![image-9](figures/9.png)
   ```C++
   // main.c
   #include <stdio.h>
      
   void top( int a[100], int b[100], int sum[100]);
      
   int main()
   {
      int a[100];
      int b[100];
      int c[100];
         
      for(int i = 0; i < 100; i++) 
      {
         a[i] = i; 
         b[i] = i * 2; 
         c[i] = 0;
      }
         
      // Call the DUT function, i.e., your adder
      top(a, b, c);
         
      // verify the results
      int pass = 1;
      for(int j = 0; j < 100; j++) 
      {
         if(c[j] != (a[j] + b[j]))
         {
            pass = 0;
         }
         printf("A[%d] = %d; B[%d] = %d; Sum C[%d] = %d\n", j, a[j], j, b[j], j, c[j]); 
      }
         
      if(pass)
         printf("Test Passed! :) \n");
      else
         printf("Test Failed :( \n");
         
      return 0;
   }
   ```
   Testbench does not get synthesized. So you are free to use any C/C++ construct for your testing purposes!
8. Let’s run C simulation for our adder module.
   ![image-10](figures/10.png)
   ![image-11](figures/11.png)
   ![image-12](figures/12.png)
   Note: Vivado C/C++ compiler is rather slow. We recommend using `g++` to run simulations!
   ```
   $ g++ main.c top.c -o vadd
   $ ./vadd
   ```
9. Now that the simulation has passed, let’s run high-level synthesis and generate the RTL for our adder. Go to `Project Settings > Synthesis`, and specify `top (top.c)` as the top function.
   ![image-13](figures/13.png)
   ![image-14](figures/14.png)
   ![image-15](figures/15.png)
   ![image-16](figures/16.png)
10. Run synthesis.
   ![image-17](figures/17.png)
11. Check the console to know when the synthesis finishes.
   ![image-18](figures/18.png)
12. We can now view the performance reports and resource utilization.
   ![image-19](figures/19.png)
13. C/RTL co-simulation can also be run at this stage. Vivado uses the same test bench `main.c` to test the RTL generated. This is left as an exercise. We will now export our adder “IP” for integration in Vivado.
   ![image-20](figures/20.png)
   ![image-21](figures/21.png)
   ![image-22](figures/22.png)
15. That’s it. We should now move to Vivado to generate the bitstream with our exported adder IP!

## CLI Makes Life Easier
When working with larger designs, it may be easier to simply work on the command line.  
1. Use `g++` compiler for functional verification.
   ```Bash
   $ g++ main.c top.c -o vadd
   $ ./vadd
   ```
2. Use the following TCL script `synth.tcl` to run the synthesis and export RTL.
   ```TCL
   # TCL commands for batch-mode HLS
   
   # Create project
   open_project proj
   
   # Set top-level design file (DUT)
   set_top top
   
   # Add source code files
   add_files top.c
   
   # Add test bench files
   add_files -tb ./main.c
   
   # Create design solution
   open_solution "solution2"
   
   # Set the FPGA board
   set_part {xczu3eg-sbva484-1-e}
   
   # Set the clock period
   create_clock -period 10 -name default
   
   ## C simulation
   # Use Makefile instead. This is even slower.
   #csim_design -O -clean
   
   ## C code synthesis to generate Verilog code
   csynth_design
   
   ## C and Verilog co-simulation
   ## This usually takes a long time so it is commented
   ## You may uncomment it if necessary
   #cosim_design
   
   ## export synthesized Verilog code
   #export_design -format ip_catalog
   
   exit
   ```
3. Invoke Vivado in batch-mode and pass the TCL file as argument.
   ```Bash
   $ faketime -f "-4y" vivado_hls synth.tcl
   ```
4. You should see something similar on your terminal.
   ![image-23](figures/23.png)   
5. To view the performance reports, open `proj/solution2/syn/report/top_csynth.rpt`.
   ![image-24](figures/24.png)
6. You can view the log file `vivado_hls.log` for any warnings. To view the synthesized RTL, go to `proj/solution2/impl/verilog/`.

## Generating Bitstream in Vivado
1. Invoke Vivado GUI.
   ```Bash
   $ vivado
   ```
   ![image-25](figures/25.png)
2. Create a new project, name it as `adder_Project`. Make sure you select the same board you're using. If you cannot find it, there is an `install board` icon at top right corner where you can install your own board.
   ![image-26](figures/26.png)
   ![image-27](figures/27.png)
   ![image-28](figures/28.png)
   ![image-29](figures/29.png)
   ![image-30](figures/30.png)
   ![image-31](figures/31.png)
   ![image-32](figures/32.png)
3. Add our adder IP core to the Vivado. Click `IP Catalog` at the left column, right click the `Vivado Repository`, and select `Add Repository`.
   ![image-33](figures/33.png)
   ![image-34](figures/34.png)
4. Select the folder that includes your HLS solution `solution1`. Then click `select` button. The following page should pop up.
   ![image-35](figures/35.png)
   ![image-36](figures/36.png)
5. Expand the IPs tab. If you see the top IP with an `orange` icon, there no issue for now. If the icon is `grey`, re-check whether the same board was chosen as in Vivado.
   ![image-37](figures/37.png)
6. Now we build the block diagram. Click the `Create Block Design` at the left column. Click the `+` icon at the upper side of the diagram. Type `hls` for finding the add function ip. Type `zynq` to find the embedded controller.
   ![image-38](figures/38.png)
   ![image-39](figures/39.png)
   ![image-40](figures/40.png)   
7. Since we specified two inputs and one output in our C code in different “bundles”, we need to initialize 3 AXI buses on the FPGA. To do so, double click the `ZYNQ` icon on the block diagram. Select `PS-PL Configuration`. Then, select the `S AXI HP0 Interface` to `S AXI HP2 Interface` under `PS-PL Interface > Slave Interface > AXI HP` by checking their boxes.
   ![image-41](figures/41.png)   
8. Go back to the Block Diagram and click `Run Connection Automation`.
   ![image-42](figures/42.png)    
9. You need to manually map the HLS ports to the three AXI HP buses in the IP.
   ![image-44](figures/44.png)
   ![image-45](figures/45.png)
   ![image-46](figures/46.png)            
10. Now select the `All Automationat` the left column. Click `OK` to start connection automation.
   ![image-47](figures/47.png)
   ![image-48](figures/48.png)   
11. To check the correctness, click the `validation` (check icon) on the upper page.
   ![image-49](figures/49.png)
   ![image-50](figures/50.png)   
12. The next step is to create a wrapper for the Block Design. Find the block diagram file under the design sources. Right click the design file (whatever you name it) and select `Create HDL Wrapper`. Choose `Let Vivado manage wrapper and auto-update` option. Click `OK` to start.
   ![image-51](figures/51.png)
   ![image-52](figures/52.png)
   ![image-53](figures/53.png)
13. Finally, click `Generate Bitstream` under `PROGRAM AND DEBUG` division (at the lower left of the entire page). Use the default settings (for our simple example) and start to run.
   ![image-54](figures/54.png)
   ![image-55](figures/55.png)
14. If bitstream generation is successful, you should be able to view the implemented design!
   ![image-56](figures/56.png)
   ![image-57](figures/57.png)
16. After generating the bitstream we need two files for running the vector addition on FPGA: the bitstream with `.bit` as the extension and the hardware handoff file with `.hwh`. You can find the `.bit` file under `adderProject/adderProject.runs/impl_1`. The `.hwh` file is under the directory `adderProject/adderProject.gen/sources_1/bd/design_1/hw_handoff`.
17. You can download these two files to a flash drive and put them on your own laptop for the next step. Note that these two files must have the same name except for the extension.

## Deploying the Adder on FPGA
1. Check this [page](http://www.pynq.io/board.html) for setting up the Ultra96V2 board. (The SD card has image already. No need to flash the SD card for now.)
2. Connect with a USB cable. Use your browser on your computer to connect to the board using the IP address of **192.168.3.1**. Jupyter login password: **xilinx**.
3. Upload the `.bit` file and the `.hwh` file to Jupyter. In the same folder, create a new `.ipynb` file for writing the script.
    - Click [here](https://pynq.readthedocs.io/en/v2.0/overlay_design_methodology/overlay_tutorial.html) to access the overlay tutorial.
4. Find the address offset of the memory ports (`a`, `b`, and `sum`, in this example). This information can be found in the xtop_hw.h file under `solution1/impl/misc/drivers/top_v1_0/src` directory.
5. Below is the example Python host code to control the FPGA kernel.
   ```Python
   import numpy as np
   import pynq
   from pynq import MMIO
   
   overlay = pynq.Overlay('adder.bit')
   
   top_ip = overlay.top_0
   top_ip.signature
   
   a_buffer = pynq.allocate((100), np.int32)
   b_buffer = pynq.allocate((100), np.int32)
   sum_buffer = pynq.allocate((100), np.int32)
   
   # initialize input
   for i in range (0, 100):
       a_buffer[i] = i
       b_buffer[i] = i+5
   
   aptr = a_buffer.physical_address
   bptr = b_buffer.physical_address
   sumptr = sum_buffer.physical_address
   
   # specify the address
   # These addresses can be found in the generated .v file: top_control_s_axi.v
   top_ip.write(0x10, aptr)
   top_ip.write(0x1c, bptr)
   top_ip.write(0x28, sumptr)
   
   
   # start the HLS kernel
   top_ip.write(0x00, 1)
   isready = top_ip.read(0x00)
   
   while( isready == 1 ):
       isready = top_ip.read(0x00)
   
   print("Array A:")
   print(a_buffer[0:10])
   print("\nArray B:")
   print(b_buffer[0:10])
   
   print("\nExpected Sum:")
   print((a_buffer + b_buffer)[0:10])
   
   print("\nFPGA returns:")
   print(sum_buffer[0:10])
   ```
