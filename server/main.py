from fastapi import FastAPI, UploadFile, File
from fastapi.middleware.cors import CORSMiddleware
from PIL import Image
import torch
from torchvision import transforms, models
import json
import io

# ==========================
# CONFIG
# ==========================
MODEL_PATH = "model_best.pth"
CLASSES = ["cardboard", "glass", "metal", "paper", "plastic", "trash"]

# ==========================
# FASTAPI SETUP
# ==========================
app = FastAPI()

app.add_middleware(
    CORSMiddleware,
    allow_origins=["*"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# ==========================
# LOAD MODEL
# ==========================
device = "cpu"

model = models.mobilenet_v2(weights=None)
model.classifier[1] = torch.nn.Linear(model.last_channel, len(CLASSES))
model.load_state_dict(torch.load(MODEL_PATH, map_location=device))
model.to(device)
model.eval()

# ==========================
# PREPROCESSING
# ==========================
transform = transforms.Compose([
    transforms.Resize((224, 224)),
    transforms.ToTensor(),
])

# ==========================
# PREDICTION ENDPOINT
# ==========================
@app.post("/predict")
async def predict(file: UploadFile = File(...)):
    # Read image
    image_bytes = await file.read()
    img = Image.open(io.BytesIO(image_bytes)).convert("RGB")

    # Preprocess
    input_tensor = transform(img).unsqueeze(0).to(device)

    # Run model
    with torch.no_grad():
        output = model(input_tensor)
        _, predicted = output.max(1)

    predicted_class = CLASSES[predicted.item()]

    return {
        "class": predicted_class
    }

# ==========================
# ROOT ENDPOINT
# ==========================
@app.get("/")
def root():
    return {"status": "ok", "message": "Waste classification API is running"}
