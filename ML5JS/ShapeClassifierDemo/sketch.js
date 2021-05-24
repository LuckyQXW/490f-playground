let shapeClassifier;
let canvas;
let clearButton;
let inputImage;
let resultDiv;

function setup() {
  canvas = createCanvas(400, 400);
  
  const options = {
    inputs: [64, 64, 4],
    task: 'imageClassification'
  }
  shapeClassifier = ml5.neuralNetwork(options);
  
  const modelDetails = {
    model: 'model/model.json',
    metadata: 'model/model_meta.json',
    weights: 'model/model.weights.bin'
  };
  background(255);
  clearButton = createButton("Clear");
  clearButton.mousePressed(function() {
    background(255);
  });
  resultDiv = createDiv("prediction: ...");

  inputImage = createGraphics(64, 64);
  shapeClassifier.load(modelDetails, modelLoaded);
  strokeWeight(20);
}

function modelLoaded(){
  // continue on your neural network journey
  // use nn.classify() for classifications or nn.predict() for regressions
  console.log("model ready!");
  classifyShape();
}

function draw() {
  if (mouseIsPressed) {
    line(pmouseX, pmouseY, mouseX, mouseY);
  }
}

function classifyShape() {
  inputImage.copy(canvas, 0, 0, 400, 400, 0, 0, 64, 64);
  shapeClassifier.classify({image: inputImage}, gotResults);
}

function gotResults(err, results) {
  if (err) {
    console.log(err);
    return;
  }
  resultDiv.html(`${results[0].label}, ${results[0].confidence}`)
  classifyShape();
}