import torch
import struct

def saveTensor2Bin(tensor, fileName):
    with open(fileName,'wb') as f:
        tensor.numpy().tofile(f)

# to add the support for padding & bias, see https://pytorch.org/docs/stable/generated/torch.nn.Conv2d.html
batchSize = 2
numInputChannels = 16
numOutputChannels = 8
stride = 2
kernelSize = 3
inputHeight = 50
inputWidth = 100

layer0 = torch.nn.Conv2d(numInputChannels, numOutputChannels, kernelSize, stride=stride, bias=False, dtype=torch.float)
input = torch.randn(batchSize, numInputChannels, inputHeight, inputWidth, dtype=torch.float)
output = layer0(input)
output = layer0(input)

saveTensor2Bin(input, 'input.bin')
saveTensor2Bin(layer0.weight.detach(), 'weight.bin')
saveTensor2Bin(output.detach(), 'output_torch.bin')

