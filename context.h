#ifndef CONTEXT
#define CONTEXT

#include<memory>
#include"configuration.h"
#include<allegro5/allegro.h>
#include<string>
#include<vector>
#include"background.h"

class Context;
//Root contains pointer to the base of the context chain,
//so use the rerootContext method to avoid memory leaks when rebasing the root
class RootContextHolder {
	public:
		RootContextHolder();
		RootContextHolder(Context* newRootContext,Context** cur);
		~RootContextHolder();
		//reroot removes old chain and starts new one with newRootContext
		void rerootContext (Context* newRootContext);
	private:
		Context* root;
		Context** current;
};

class Context {
	public:
		Context(PlayerConfiguration* curConfig);
		virtual void frameStep (double deltaTime)=0;
		virtual void eventStep (ALLEGRO_EVENT* event)=0;
		virtual ~Context ();
		//use macro VIRTUAL_CONTEXT_COPY for standard implementation of copy method
		virtual Context* copy ()=0;
		template<class T> static inline Context* defaultCopy (T* self) {
			return (Context*) new T(*self);
		}
	protected:
		PlayerConfiguration* currentConfig;
};

class OuterContext : public Context {
	public:
		OuterContext(PlayerConfiguration*);
		OuterContext(const OuterContext& original);
		OuterContext& operator=(const OuterContext& rhs);
		void passControl (Context* newInnerContext);
		void releaseInner ();
		Context* getInner();
		//drawStep is for any inner Context that is aware of the OuterContext, so that it can draw
		//the OuterContext without having it's frameStep execute
		virtual void drawStep ()=0;
		virtual void outerControl()=0;
	private:
		std::unique_ptr<Context> innerContext;
};

class ChoiceBox : public Context {
	public:
		ChoiceBox(PlayerConfiguration*,OuterContext* outer,std::vector<std::string> textChoices, Background* textBg, bool pauseOuter,
				unsigned int boxWidth, unsigned int boxHeight, unsigned int x=0, unsigned int y=0, unsigned int nCols=0,
				unsigned int nRows=0, float textAlignmentX=0, float textAlignmentY=0, unsigned int textPadX=1,
				unsigned int textPadY=1, unsigned int currentChoice=0);
		ChoiceBox(const ChoiceBox& original);
		ChoiceBox& operator=(const ChoiceBox& rhs);
		virtual void drawStep ();
		virtual void frameStep (double); 
		virtual void eventStep (ALLEGRO_EVENT*);
		virtual Context* copy ();
		void draw ();
		unsigned int curChoice;
		bool pause;
	protected:
		OuterContext* outerContext;
		unsigned int rows;
		unsigned int cols;
		unsigned int width;
		unsigned int height;
		unsigned int xOrigin;
		unsigned int yOrigin;
		unsigned int padX;
		unsigned int padY;
		//alignments are ratios in the range of 0 and 1.0
		float alignmentX;
		float alignmentY;
		std::vector<std::string> choices;
		std::unique_ptr<Background> bg;
};

#endif
