let canvas;
let count = 0;
let circles = [];
let squares = [];
let triangles = [];
let shapeClassifier;

function preload() {
  console.log("loading data");
  for (let i = 0; i < 500; i++) {
    let index = nf(i + 1, 4, 0);
    circles[i] = loadImage(`data/circle${index}.png`);
    squares[i] = loadImage(`data/square${index}.png`);
    triangles[i] = loadImage(`data/triangle${index}.png`);
  }
  console.log("data loaded");
}
function setup() {
  canvas = createCanvas(64, 64);
  background(255);
  image(circles[0], 0, 0, width, height);

  let options = {
    inputs: [64, 64, 4],
    task: "imageClassification",
    debug: true
  };
  shapeClassifier = ml5.neuralNetwork(options);
  
  for (let i = 0; i < circles.length; i++) {
    shapeClassifier.addData({image: circles[i]}, {label: "circle"});
    shapeClassifier.addData({image: squares[i]}, {label: "square"});
    shapeClassifier.addData({image: triangles[i]}, {label: "triangle"});
  }
  shapeClassifier.normalizeData();
  shapeClassifier.train( {epochs: 50}, finishTraining);
}

function finishTraining() {
  console.log("finished training");
  shapeClassifier.save();
}

function draw() {
}
