#pragma once
#include "NodeView.h"

class Renderable {
public:
	virtual ~Renderable() {};
	virtual NodeView::Data Render() const = 0;
	virtual NodeView::NodeType Type() const = 0;
};