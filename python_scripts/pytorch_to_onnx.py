# -----------------------------------------------------------
# demonstrates how to convert pytorch model to ONNX
# -----------------------------------------------------------

import torch
import torchvision

BEST_MODEL_PATH_PT = 'artifacts/best_model.pth'
BEST_MODEL_PATH_ONNX = 'artifacts/best_model.onnx'

#Load pre-trained Model
model = torchvision.models.alexnet(pretrained=False)
model.classifier[6] = torch.nn.Linear(model.classifier[6].in_features, 2)
model.load_state_dict(torch.load(BEST_MODEL_PATH_PT))

# Export the model from PyTorch to ONNX
model.eval()
x = torch.randn(1, 3, 224, 224, requires_grad=True)
y = model(x)
torch_out = torch.onnx._export(model,             # model being run
                                x,          # model input (or a tuple for multiple inputs)
                                BEST_MODEL_PATH_ONNX,      # where to save the model (can be a file or file-like object)
                                export_params=True,       # store the trained parameter weights inside the model file
                                input_names=['Conv2d0_3-64'],     # specify the name of input layer in onnx model
                                output_names=['Linear2_4096-2'])    # specify the name of output layer

print("Successfully genereated ONNX model at ",BEST_MODEL_PATH_ONNX)