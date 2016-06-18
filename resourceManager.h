#ifndef RESOURCE_MANAGER
#define RESOURCE_MANAGER
#include<string>
#include<allegro5/allegro.h>
#include<allegro5/allegro_font.h>

#define MANAGER_DECLARE_RESOURCE(T,TNAME,LOAD_FUNCTION_EXTRA_ARGS) \
	struct TNAME ## Entry; \
	class TNAME ## Wrapper { \
		public: \
			TNAME ## Wrapper (); \
			TNAME ## Wrapper (const ResourceManager:: TNAME ## Wrapper&); \
			~TNAME ## Wrapper (); \
			T* data; \
			ResourceManager:: TNAME ## Wrapper& operator=(const ResourceManager:: TNAME ## Wrapper & rhs); \
			operator T*(); \
		private: \
			friend ResourceManager:: TNAME ## Wrapper load ## TNAME (std::string LOAD_FUNCTION_EXTRA_ARGS); \
			TNAME ## Wrapper (ResourceManager:: TNAME ## Entry*); \
			TNAME ## Entry* entry; \
	}; \
	TNAME ## Wrapper load ## TNAME (std::string fileName LOAD_FUNCTION_EXTRA_ARGS);
	

#define RESOURCE_LD_ARGS_COM_SEP ,

namespace ResourceManager {
	MANAGER_DECLARE_RESOURCE(ALLEGRO_BITMAP,Image,)
	MANAGER_DECLARE_RESOURCE(ALLEGRO_FONT,Font,RESOURCE_LD_ARGS_COM_SEP int RESOURCE_LD_ARGS_COM_SEP int)
}

#undef RESOURCE_LD_ARGS_COM_SEP

#endif
