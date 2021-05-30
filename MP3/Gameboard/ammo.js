const AMMO_SIZE = 20;
class Ammo extends Shape {
  constructor(x, y, shape) {
    super(x, y, AMMO_SIZE, AMMO_SIZE);
    this.angle = 0;
    this.speed = 2;
    this.shape = shape;
    this.active = true;
  }
  
  update() {
    // Shoot up
    this.y -= this.speed;
  }
  
  setActive(active) {
    this.active = active;
  }
  
  isActive() {
    return this.active;
  }
  
  getShape() {
    return this.shape;
  }
  
  draw() {
    push();
      translate(this.x, this.y);
      fill(255);
      switch(this.shape) {
        case 0:
          circle(SHAPE_SIZE / 2, SHAPE_SIZE / 2, SHAPE_SIZE);
          break;
        case 1:
          rect(0, 0, SHAPE_SIZE, SHAPE_SIZE);
          break;
        case 2:
          triangle(0, SHAPE_SIZE, SHAPE_SIZE / 2, 0, SHAPE_SIZE, SHAPE_SIZE);
          break;
      }
    pop();
  }
}