# Example: using a C function to implement one CONV layer

For running the tutorial, Pytorch as well as the gcc/g++ compiler are needed.

First, run the Pytorch code to generate input/weight binaries (as the input of the C function) and output binary (as the ground truth of the C function).

```
python main.py
```

Second, compile and run the C function by:

```
rm -rf a.out && g++ main.cpp && ./a.out
```

You should see a "PASSED" if the C-function-generated output match the ground truth.

To get more familarity, you are welcome to look into the code, play with the CONV parameters (please keep C & torch config consistent), and add more features (e.g., bias, padding).

