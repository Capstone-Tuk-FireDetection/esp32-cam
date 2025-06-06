import torch
from torchvision import transforms
from PIL import Image
from typing import Callable


def load_flame_classifier(model_path: str, device: str = "cpu") -> Callable[[str], str]:
    """Load a PyTorch model and return an image classification function.

    The returned function takes a path to an image file and returns
    "flame" or "no_flame" based on the model's prediction.
    """
    model = torch.load(model_path, map_location=device)
    if isinstance(model, dict) or not hasattr(model, "eval"):
        raise ValueError(
            "Model file must contain a serialized torch.nn.Module. "
            "Did you save only the state_dict?"
        )

    model.eval()
    model.to(device)

    preprocess = transforms.Compose([
        transforms.Resize((224, 224)),
        transforms.ToTensor(),
    ])
    labels = ["no_flame", "flame"]

    def classify(image_path: str) -> str:
        image = Image.open(image_path).convert("RGB")
        tensor = preprocess(image).unsqueeze(0).to(device)
        with torch.no_grad():
            output = model(tensor)
            pred = output.argmax(dim=1).item()
        return labels[pred]

    return classify
