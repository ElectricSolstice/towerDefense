#include"context.h"
#include"debug.h"

RootContextHolder::RootContextHolder () {
	root = NULL;
}
RootContextHolder::RootContextHolder (Context* newRootContext,Context** cur) {
	root = newRootContext;
	current = cur;
}
RootContextHolder::~RootContextHolder () {
	delete root;
}
void RootContextHolder::rerootContext (Context* newRootContext) {
	(*current)=newRootContext;
	delete root;
	root = newRootContext;
}

Context::Context(PlayerConfiguration* curConfig) {
	currentConfig = curConfig;
}
Context::~Context() {}

OuterContext::OuterContext(PlayerConfiguration* curConfig) : Context(curConfig) {}
OuterContext::OuterContext(const OuterContext& original) : Context (currentConfig) {
	if (original.innerContext) {
		innerContext.reset(innerContext->copy());
	}
}
OuterContext& OuterContext::operator=(const OuterContext& rhs) {
	Context::operator=(rhs);
	if (rhs.innerContext) {
		innerContext.reset(innerContext->copy());
	}
	return *this;
}
void OuterContext::passControl (Context* newOuterContext) {
	innerContext.reset(newOuterContext);
	(*currentConfig->currentContext) = innerContext.get();
}
void OuterContext::releaseInner () {
	innerContext.reset();
	(*currentConfig->currentContext) = this;
}

Context* OuterContext::getInner() {
	return innerContext.get();
}
ChoiceBox::ChoiceBox(PlayerConfiguration* curConfig,OuterContext* outer,std::vector<std::string> textChoices,
		Background* textBg, bool pauseOuter, unsigned int boxWidth, unsigned int boxHeight, unsigned int x,
		unsigned int y, unsigned int nCols, unsigned int nRows, float textAlignmentX, float textAlignmentY,
	       	unsigned int textPadX, unsigned int textPadY, unsigned int currentChoice) : Context(curConfig) {
	outerContext=outer;
	width = boxWidth;
	height = boxHeight;
	rows = nRows;
	cols = nCols;
	choices = textChoices;
	pause = pauseOuter;
	xOrigin=x;
	yOrigin=y;
	curChoice = currentChoice;
	padX=textPadX;
	padY=textPadY;
	bg.reset(textBg);
	alignmentX = textAlignmentX;
	alignmentY = textAlignmentY;
	assert(alignmentX <= 1.0);
	assert(alignmentX >= 0.0);
	assert(alignmentY <= 1.0);
	assert(alignmentY >= 0.0);
	if (rows == 0) {
		unsigned int largestCharIndex=0;
		for (unsigned int i=1;i<choices.size();++i) {
			if (choices[i].length() > choices[largestCharIndex].length()) {
				largestCharIndex=i;
			}
		}
		rows = width/al_get_text_width(currentConfig->font.data,choices[largestCharIndex].c_str());
		//since rows is an unsigned int, make sure it's at least 1 if the division of the width results in 0
		if (rows == 0) {
			rows=1;
		}
	} 
	if (cols == 0) {
		unsigned int padSize = al_get_text_width(currentConfig->font.data," ");
		cols = height/(al_get_font_line_height(currentConfig->font.data)+padY*padSize);
	}
}
void ChoiceBox::draw () {
	ALLEGRO_FONT* font = currentConfig->font.data;
	ALLEGRO_COLOR color = currentConfig->textColor;
	unsigned int startIndex=0;
	unsigned int boxSize = rows*cols;
	unsigned int textWidth=0;
	unsigned int textHeight=0;
	unsigned int padSize = al_get_text_width(font," ");
	unsigned int tHeight=al_get_font_line_height(font)+padY*padSize;
	if (tHeight < height/rows) {
		tHeight = height/rows;
	}
	unsigned int tWidth= width/cols;
	unsigned int offsetX = ((unsigned int)(alignmentX*width));
	unsigned int offsetY = ((unsigned int)(alignmentY*height));
	if (offsetX > xOrigin) {
		offsetX=0;
	} else {
		offsetX=xOrigin-offsetX;
	}
	if (offsetY> yOrigin) {
		offsetY=0;
	} else {
		offsetY=yOrigin-offsetY;
	}
	while (curChoice >= boxSize+startIndex*boxSize) {
		startIndex+=1;
	}
	for (unsigned int i=0;i<choices.size() && i<boxSize;++i) {
		if (i==(curChoice-startIndex*boxSize)) {
			color=currentConfig->selectTextColor;
			font = currentConfig->selectFont.data;
		} 
		if (i%cols==0 && i!=0) {
			textHeight+=tHeight;
		}
		textWidth=tWidth*(i%cols);
		al_draw_text(font,color,offsetX+textWidth,offsetY+textHeight,0,choices[i+startIndex*boxSize].c_str());
		if (i==(curChoice-startIndex*boxSize)) {
			color=currentConfig->textColor;
			font = currentConfig->font.data;
		}
	}
}
void ChoiceBox::drawStep () {
	outerContext->drawStep();
	draw();
}
void ChoiceBox::frameStep (double deltaTime) {
	if (pause) {
		drawStep();
	} else {
		outerContext->frameStep(deltaTime);
		draw();
	}
}
void ChoiceBox::eventStep (ALLEGRO_EVENT* event) {
	if (event->type == ALLEGRO_EVENT_KEY_UP) {
		switch(event->keyboard.keycode) {
			case ALLEGRO_KEY_UP:
				if (curChoice>=cols) {
					curChoice-=cols;
				}
				break;
			case ALLEGRO_KEY_DOWN:
				if (curChoice+cols<choices.size()) {
					curChoice+=cols;
				}
				break;
			case ALLEGRO_KEY_LEFT:
				if (curChoice>0) {
					--curChoice;
				}
				break;
			case ALLEGRO_KEY_RIGHT:
				if (curChoice+1<choices.size()) {
					++curChoice;
				}
				break;
			case ALLEGRO_KEY_SPACE:
				outerContext->outerControl();
				break;
		}
	} else if (event->type == ALLEGRO_EVENT_DISPLAY_CLOSE || (event->type==ALLEGRO_EVENT_KEY_DOWN && event->keyboard.keycode == ALLEGRO_KEY_ESCAPE)) {
		outerContext->eventStep(event);
	} else if (event->type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
		float mouseX = event->mouse.x;
		float mouseY = event->mouse.y;
		unsigned int offsetY = alignmentY*height;
		unsigned int offsetX = alignmentX*width;
		if (mouseX > xOrigin-offsetX && mouseY > yOrigin-offsetY && mouseX < xOrigin-offsetX+width && mouseY < yOrigin-offsetY+height) {
			mouseX-=(xOrigin-offsetX);
			mouseY-=(yOrigin-offsetY);
			float choiceWidth = width/cols;
			float choiceHeight = height/rows;
			int colIndex = mouseX/choiceWidth;
			int rowIndex = mouseY/choiceHeight;
			curChoice=colIndex+(rowIndex*cols);
			outerContext->outerControl();
		}
	}
}
ChoiceBox::ChoiceBox (const ChoiceBox& original) : Context(original) {
	if (original.bg) {
		bg.reset(bg->copy());
	}
}
ChoiceBox& ChoiceBox::operator=(const ChoiceBox& rhs) {
	Context::operator=(rhs);
	if (rhs.bg) {
		bg.reset(bg->copy());
	}
	return *this;
}
Context* ChoiceBox::copy () {
	return Context::defaultCopy<ChoiceBox>(this);
}
