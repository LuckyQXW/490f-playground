const CANNON_WIDTH = 40;
const CANNON_HEIGHT = 60;
class Cannon extends Shape {
  constructor(x, y) {
    super(x, y, CANNON_WIDTH, CANNON_HEIGHT);
    this.ammos = [];
    this.capacity = 2;
    this.active = false;
    this.shootInterval = 20;
    this.lastShootTime = 0;
    this.shootEffect = createAudio('assets/shoot.wav');
    this.shootEffect.volume(0.5);
  }
  
  update() {
  }
  
  shoot(gameAmmos) {
    if ((frameCount - this.lastShootTime) >= this.shootInterval && this.ammos.length > 0) {
      this.shootEffect.play();
      let shape = this.ammos.pop();
      let ammo = new Ammo(this.x + 10, this.y, shape);
      gameAmmos.push(ammo);
      this.lastShootTime = frameCount;
    }
  }
  
  setActive(active) {
    this.active = active;
  }
  
  addAmmo(shape) {
    if (this.ammos.length < this.capacity) {
      this.ammos.push(shape);
    }
  }
  
  reset() {
    this.ammos = [];
    this.lastShootTime = frameCount;
  }
  
  draw() {
    push();
      stroke(255, 0, 0);
      if (this.active) {
        strokeWeight(5);
      } else {
        noStroke();
      }
      translate(this.x, this.y);
      fill(0);
      if (this.ammos.length > 0) {
        if (this.ammos[0] == 0) {
          fill('red');
        } else if (this.ammos[0] == 1) {
          fill('yellow');
        } else if (this.ammos[0] == 2) {
          fill('green');
        } else {
          fill(0);
        }
      }
      rect(0, 0, CANNON_WIDTH, CANNON_HEIGHT / 2);
      fill(0);
      if (this.ammos.length > 1) {
        if (this.ammos[1] == 0) {
          fill('red');
        } else if (this.ammos[1] == 1) {
          fill('yellow');
        } else if (this.ammos[1] == 2) {
          fill('green');
        } else {
          fill(0);
        }
      }
      rect(0, CANNON_HEIGHT / 2, CANNON_WIDTH, CANNON_HEIGHT / 2);
    pop();
  }
}