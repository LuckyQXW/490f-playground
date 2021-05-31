void setup() {
  size(64, 64);
  strokeWeight(4);
}


void draw() {
  background(255);
  saveFrame("data/empty####.png");
  /*
  for (int i = 0; i < 3; i++) {
    background(255);
    pushMatrix();
    float r = random(8, 20);
    float centerX = width / 2;
    float centerY = height / 2;
    if (frameCount < 250) {
      if (i == 0) {
        circle(centerX, centerY, r*2);
        saveFrame("data/circle####.png");
      } else if (i == 1) {
        rectMode(CENTER);
        rotate(random(-0.1, 0.1));
        rect(centerX, centerY, r * 2 - random(-2, 2), r * 2 - random(-2, 2));
        saveFrame("data/square####.png");
      } else if (i == 2) {
        rotate(random(-0.1, 0.1));
        triangle(centerX, centerY-r, centerX-r, centerY + r, centerX + r, centerY + r);
        saveFrame("data/triangle####.png");
      }
    } else {
      float x = random(r, width - r - 2);
      float y = random(r, height - r - 2);
      translate(x, y);
      if (i == 0) {
        circle(0, 0, r*2);
        saveFrame("data/circle####.png");
      } else if (i == 1) {
        rectMode(CENTER);
        rotate(random(-1, 1));
        rect(0, 0, r * 2 - random(-2, 2), r * 2 - random(-2, 2));
        saveFrame("data/square####.png");
      } else if (i == 2) {
        rotate(random(-1, 1));
        triangle(0, -r, -r, r, r, r);
        saveFrame("data/triangle####.png");
      }
    }
    popMatrix();
  }
  */
  if (frameCount == 5) {
    exit();
  }
}
