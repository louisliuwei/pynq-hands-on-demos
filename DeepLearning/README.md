# BNN-PYNQ

This repo contains the pip install package for Quantized Neural Network (QNN) on PYNQ. 
Two different network topologies are here included, namely CNV and LFC as described in the <a href="https://arxiv.org/abs/1612.07119" target="_blank"> FINN Paper </a>. 
Now, there are multiple implementations available supporting different precision for weights and activation:

- 1 bit weights and 1 bit activation (W1A1) for CNV and LFC
- 1 bit weights and 2 bit activation (W1A2) for CNV and LFC
- 2 bit weights and 2 bit activation (W2A2) for CNV

## Install
Install by typing: 
> on PYNQ v2.3
```
sudo pip3 install -e .
```
> on PYNQ v2.2 and earlier
```
sudo pip3.6 install -e . 
```
into the terminal on your Pynq-Z1/Pynq-Z2 board. 

This will install the BNN package to your board, and create a **bnn** directory in the Jupyter home area. You will find the Jupyter notebooks to test the networks in this directory.

## Run Road-signs Demo
Open http://192.168.2.99:9090/notebooks/bnn/CNV-BNN_Road-Signs.ipynb using your browser.