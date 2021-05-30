let cannons = [];
let activeCannon = 0;
const numLanes = 5;
let sadShapes = [];
let ammos = [];
// Used to keep track of which lane has shapes incoming
let shapeCounts = {0:0,1:0,2:0,3:0,4:0}
let lives = 3;

let isGameOver = false;
let hasGameBegun = false;
let score = 0;
let highScore = -1;
let lastHighScore = -1;
let stage = 1;
// Used to keep track of time
let lastFrameCount = 0;
// Shapes are spawned every 100 frames
let shapeSpawnInterval = 100;

function preload() {
  
}

function setup() {
  createCanvas(500, 400);
  // Set up cannons
  let x = 30;
  for (let i = 0; i < numLanes; i++) {
    cannons.push(new Cannon(x, 340));
    x += 100;
  }
  resetGame();
  noLoop();
}

function resetGame() {
  score = 0;
  lives = 3;
  shapeCounts = {0:0,1:0,2:0,3:0,4:0};
  sadShapes = [];
  ammos = [];
  for (let i = 0; i < numLanes; i++) {
    cannons[i].reset();
  }
  isGameOver = false;
  loop();
}

function keyPressed() {
  if (key == ' ') {
    if (isGameOver) {
      resetGame();
    } else if (!hasGameBegun) {
      hasGameBegun = true;
      loop();
    }
  }
  if (keyIsDown(RIGHT_ARROW)) {
    activeCannon = (activeCannon + 1) % numLanes;
  } else if (keyIsDown(LEFT_ARROW)) {
    activeCannon--;
    if (activeCannon < 0) {
      activeCannon = numLanes - 1;
    }
  } 
  // TODO: replace by the canvas input
  else if (key === '1') {
    cannons[activeCannon].addAmmo(1);
  } else if (key === '2') {
    cannons[activeCannon].addAmmo(2);
  } else if (key === '0') {
    cannons[activeCannon].addAmmo(0);
  }
}

function draw() {
  background(220);
  checkSpawn();
  updateAmmos();
  updateSadShapes();
  updateCannons();
  drawScore();
}

function updateCannons() {
  for (let i = 0; i < numLanes; i++) {
    // Can only load active cannon
    cannons[i].setActive(activeCannon == i);
    cannons[i].draw();
    // Shoot all ammos if the lane has shape incoming
    if (shapeCounts[i] > 0) {
      cannons[i].shoot(ammos);
    }
  }
}

function updateAmmos() {
  let newAmmos = []
  for (let i = 0; i < ammos.length; i++) {
    ammos[i].update();
    for (let j = 0; j < sadShapes.length; j++) {
      // check if the ammo hits any shape
      if (ammos[i].overlaps(sadShapes[j]) && ammos[i].getShape() == sadShapes[j].getShape()) {
        ammos[i].setActive(false);
        sadShapes[j].setActive(false);
        shapeCounts[sadShapes[j].getLane()] -= 1;
        score += 100;
      }
    }
    // Only keep ammo in screen and not yet collided with any shapes
    if (ammos[i].getBottom() > 0 && ammos[i].isActive()) {
      newAmmos.push(ammos[i]);
      ammos[i].draw();
    }
  }
  ammos = newAmmos;
}

function updateSadShapes() {
  let newSadShapes = []
  for (let i = 0; i < sadShapes.length; i++) {
    sadShapes[i].update();
    if (sadShapes[i].getBottom() < height - 60 && sadShapes[i].isActive()) {
      newSadShapes.push(sadShapes[i]);
      sadShapes[i].draw();
    } else if (sadShapes[i].getBottom() >= height - 60) {
      shapeCounts[sadShapes[i].getLane()] -= 1;
      // Once any shape reaches a cannon, lives -1
      lives--;
      if (lives == 0) {
        isGameOver = true;
        lastHighScore = highScore;
        if(highScore < score){
          highScore = score;
        }
        noLoop();
      }
    }
  }
  sadShapes = newSadShapes;
}

function checkSpawn() {
  if ((frameCount - lastFrameCount) >= shapeSpawnInterval) {
    spawnSadShape();
    lastFrameCount = frameCount;
  }
}

function spawnSadShape() {
  // Randomly pick a lane to spawn a random shape
  let lane = Math.floor(random(numLanes));
  let type = Math.floor(random(3));
  shapeCounts[lane] += 1;
  let x = lane * 100 + 40;
  switch(type) {
    case 0:
      sadShapes.push(new SadShape(x, 0, 0, lane));
      break;
    case 1:
      sadShapes.push(new SadShape(x, 0, 1, lane));
      break;
    case 2:
      sadShapes.push(new SadShape(x, 0, 2, lane));
      break;
  }
}

function drawScore() {
  fill(0);
  textAlign(LEFT);
  textSize(15);
  text('Score: ' + score, 10, 20);
  fill(0);
  textAlign(RIGHT);
  textSize(15);
  text('Lives: ' + lives, width - 10, 20);
  if (isGameOver) {
    // dark overlay
    fill(0, 0, 0, 100);
    rect(0, 0, width, height);

    // draw gameover text
    textAlign(CENTER);
    textSize(35);
    fill(255);
    text('GAME OVER!', width / 2, height / 3);
    
    textSize(12);
    let yText = height / 2;
    if(highScore > lastHighScore && highScore > 0) {
      text('New Hi-Score of ' + highScore + '!', width / 2, yText);
      yText += 30;
    }
    text('Press SPACE BAR to play again.', width / 2, yText);
  } else if (!hasGameBegun){
    // if we're here, then the game has yet to begin for the first time
    // dark overlay
    fill(0, 0, 0, 100);
    rect(0, 0, width, height);

    // draw game over text
    textAlign(CENTER);
    textSize(15);
    fill(255);
    text('Press SPACE BAR to play!', width / 2, height / 3);
  }
}