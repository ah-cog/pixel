void setupMenu() {
  color activeColor = color(0,130,164);
  controlP5 = new ControlP5(this);
  //controlP5.setAutoDraw(false);
  controlP5.setColorActive(activeColor);
  controlP5.setColorBackground(color(170));
  controlP5.setColorForeground(color(50));
  controlP5.setColorLabel(color(50));
  controlP5.setColorValue(color(255));

  ControlGroup ctrl = controlP5.addGroup("menu",15,25,35);
  ctrl.setColorLabel(color(255));
  ctrl.close();

  sliders = new Slider[40];
  ranges = new Range[20];
  toggles = new Toggle[40];

  int left = 0;
  int top = 5;
  int len = 300;

  int si = 0;
  int ri = 0;
  int ti = 0;
  int posY = 0;

  sliders[si++] = controlP5.addSlider("offset",0,100,left,top+posY+0,len,15);
  sliders[si++] = controlP5.addSlider("size",0,100,left,top+posY+20,len,15);
//  posY += 20;
//  sliders[si++] = controlP5.addSlider("layerOneAngle",0,TWO_PI,left,top+posY+20,len,15);
//  sliders[si++] = controlP5.addSlider("layerOneLength",0,TWO_PI,left,top+posY+40,len,15);
//  sliders[si++] = controlP5.addSlider("layerOneTransparency",0,255,left,top+posY+60,len,15);
//  sliders[si++] = controlP5.addSlider("layerOneFillGap",0,255,left,top+posY+80,len,15);
//  sliders[si++] = controlP5.addSlider("layerTwoAngle",0,TWO_PI,left,top+posY+100,len,15);
//  sliders[si++] = controlP5.addSlider("layerTwoLength",0,TWO_PI,left,top+posY+120,len,15);
//  sliders[si++] = controlP5.addSlider("layerTwoTransparency",0,255,left,top+posY+140,len,15);
//  sliders[si++] = controlP5.addSlider("layerTwoFillGap",0,255,left,top+posY+160,len,15);
//  sliders[si++] = controlP5.addSlider("layerThreeAngle",0,TWO_PI,left,top+posY+180,len,15);
//  sliders[si++] = controlP5.addSlider("layerThreeLength",0,TWO_PI,left,top+posY+200,len,15);
//  sliders[si++] = controlP5.addSlider("layerThreeTransparency",0,255,left,top+posY+220,len,15);
//  sliders[si++] = controlP5.addSlider("layerThickness",0,200,left,top+posY+240,len,15);
//  sliders[si++] = controlP5.addSlider("layerThreeFillGap",0,255,left,top+posY+260,len,15);
//  sliders[si++] = controlP5.addSlider("layerOffset",0,TWO_PI,left,top+posY+280,len,15);
//  posY += 140;
//  toggles[ti] = controlP5.addToggle("enableSketchiness",enableSketchiness,left+0,top+posY+160,15,15);
//    toggles[ti++].setLabel("enable sketchiness");
//  sliders[si++] = controlP5.addSlider("roughness",0,100,left,top+posY+180,len,15);
//  sliders[si++] = controlP5.addSlider("fillGap",0,50,left,top+posY+200,len,15);
//  sliders[si++] = controlP5.addSlider("arcSegmentFrequency",2,500,left,top+posY+220,len,15);
//  toggles[ti] = controlP5.addToggle("enableOutline",enableOutline,left+0,top+posY+240,15,15);
//    toggles[ti++].setLabel("enable outline");
//  sliders[si++] = controlP5.addSlider("strokeWeight",1,500,left,top+posY+260,len,15);
//  toggles[ti] = controlP5.addToggle("enableYvesKleinArm",enableYvesKleinArm,left+0,top+posY+280,15,15);
//    toggles[ti++].setLabel("enable second arm blue");
//  sliders[si++] = controlP5.addSlider("secondArmAngleOffset",0,360,left,top+posY+300,len,15);
//  sliders[si++] = controlP5.addSlider("cLayerCount",1,3,left,top+posY+320,len,15);
//  sliders[si++] = controlP5.addSlider("xOffset",-100,100,left,top+posY+340,len,15);
//  sliders[si++] = controlP5.addSlider("yOffset",-100,100,left,top+posY+360,len,15);
//  posY += 50;

  for (int i = 0; i < si; i++) {
    sliders[i].setGroup(ctrl);
    sliders[i].setId(i);
    sliders[i].captionLabel().toUpperCase(true);
    sliders[i].captionLabel().style().padding(4,3,3,3);
    sliders[i].captionLabel().style().marginTop = -4;
    sliders[i].captionLabel().style().marginLeft = 0;
    sliders[i].captionLabel().style().marginRight = -14;
    sliders[i].captionLabel().setColorBackground(0x99ffffff);
  }

  for (int i = 0; i < ri; i++) {
    ranges[i].setGroup(ctrl);
    ranges[i].setId(i);
    ranges[i].captionLabel().toUpperCase(true);
    ranges[i].captionLabel().style().padding(4,3,3,3);
    ranges[i].captionLabel().style().marginTop = -4;
    ranges[i].captionLabel().setColorBackground(0x99ffffff);
  }
  for (int i = 0; i < ti; i++) {
    toggles[i].setGroup(ctrl);
    toggles[i].setColorLabel(color(50));
    toggles[i].captionLabel().style().padding(4,3,3,3);
    toggles[i].captionLabel().style().marginTop = -20;
    toggles[i].captionLabel().style().marginLeft = 18;
    toggles[i].captionLabel().style().marginRight = 5;
    toggles[i].captionLabel().setColorBackground(0x99ffffff);
  }
}

void drawGUI(){
  controlP5.show(); 
  controlP5.draw();
}


// called on every change of the gui
void controlEvent(ControlEvent theControlEvent) {
  // println("got a control event from controller with id "+theControlEvent.controller().id()); 
//  initialize = true;
}

















