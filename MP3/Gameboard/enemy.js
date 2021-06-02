const SHAPE_SIZE = 20;

class Enemy extends Shape {
  constructor(x, y, shape, lane) {
    super(x, y, SHAPE_SIZE, SHAPE_SIZE);
    this.speed = 1;
    this.active = true;
    this.lane = lane;
    this.shape = shape;
  }
  
  update() {
    // Move down the lane
    this.y += this.speed;
  }
  
  isActive() {
    return this.active;
  }
  
  setActive(active) {
    this.active = active;
  }
  
  getLane() {
    return this.lane;
  }
  
  getShape() {
    return this.shape;
  }
  
  draw() {
    push();
      translate(this.x, this.y);
      fill(0);
      switch (this.shape) {
        case 0:
          fill('red')
          circle(SHAPE_SIZE / 2, SHAPE_SIZE / 2, SHAPE_SIZE);
          break;
        case 1:
          fill('yellow')
          rect(0, 0, SHAPE_SIZE, SHAPE_SIZE);
          break;
        case 2:
          fill('green')
          triangle(0, SHAPE_SIZE, SHAPE_SIZE / 2, 0, SHAPE_SIZE, SHAPE_SIZE);
          break;
      }
    pop();
  }
}