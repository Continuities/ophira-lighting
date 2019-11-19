#include "Arduino.h"
#include "LayerEngine.h"

VisualLayer::VisualLayer(int width, int height, Palette palette) {
  this->width = width;
  this->height = height;
  this->palette = palette;
  this->next = NULL;
}
void VisualLayer::apply(HSV** frame) {}

LayerEngine::LayerEngine(int width, int height) {
  this->width = width;
  this->height = height;
  this->first = NULL;
}

VisualLayer* LayerEngine::last() {
  VisualLayer* current = this->first;
  while (current && current->next) {
    current = current->next;
  }
  return current;
}

void LayerEngine::computeFrame(HSV** frame) {
  VisualLayer* currentLayer = first;
  while(currentLayer) {
    currentLayer->apply(frame);
    currentLayer = currentLayer->next;
  }
}

void LayerEngine::push(VisualLayer* layer) {
  if (this->first == NULL) {
    this->first = layer;
  }
  else {
    last()->next = layer;
  }
}

void LayerEngine::pop() {
  if (this->first != NULL && this->first->next == NULL) {
    this->first = NULL;
    return;
  }

  VisualLayer* current = this->first;
  while (current->next != NULL && current->next->next != NULL) {
    current = current->next;
  }
  current->next = NULL;
}