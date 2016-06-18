/*
 * File defines a useful specific macro for defining resources specifically when the ResourceManager namespace is open. 
 * Only use in the translation unit (cpp file) where the resource is to be defined. This shouldn't need to be included in any other
 * files, especially headers.
 */

#include<map>
#define MANAGER_DEFINE_RESOURCE(T,TNAME,LOAD_FUNCTION,RELEASE_FUNCTION,KEY_USED,LOAD_FUNCTION_EXTRA_ARGS) \
	struct TNAME ## Entry { \
		std::string key; \
		T* data; \
		unsigned int userCount; \
	}; \
	static std::map<std::string, TNAME ## Entry> TNAME ## s; \
	ResourceManager:: TNAME ## Wrapper:: TNAME ## Wrapper () { \
		data=NULL; \
	} \
	ResourceManager:: TNAME ## Wrapper:: TNAME ## Wrapper (const ResourceManager:: TNAME ## Wrapper& original) { \
		if (original.data != NULL) { \
			entry = original.entry; \
			entry->userCount++; \
			data = original.data;\
		} else { \
			data = NULL; \
		} \
	} \
	ResourceManager:: TNAME ## Wrapper::~ TNAME ## Wrapper () { \
		if (data != NULL) { \
			entry->userCount--; \
			if (entry->userCount==0) { \
				ResourceManager:: TNAME ## s.erase(entry->key); \
				RELEASE_FUNCTION; \
			} \
		} \
	} \
	ResourceManager:: TNAME ## Wrapper:: TNAME ## Wrapper (ResourceManager:: TNAME ## Entry* ent)  { \
		if (ent != NULL) { \
			data = ent->data; \
			ent->userCount++; \
		} \
		entry = ent; \
	} \
	ResourceManager:: TNAME ## Wrapper& ResourceManager:: TNAME ## Wrapper::operator= (const ResourceManager:: TNAME ## Wrapper& rhs) { \
		if (this == &rhs) { \
			return *this; \
		} \
		data = rhs.data; \
		if (data != NULL) { \
			entry = rhs.entry; \
			entry->userCount++; \
		} \
		return *this; \
	} \
	ResourceManager:: TNAME ## Wrapper::operator T*() { \
		return data;\
	}\
	ResourceManager:: TNAME ## Wrapper load ## TNAME (std::string fileName LOAD_FUNCTION_EXTRA_ARGS) { \
		std::map<std::string,ResourceManager:: TNAME ## Entry>::iterator it; \
		it = ResourceManager::TNAME ## s.find(KEY_USED); \
		if (it != ResourceManager:: TNAME ## s.end()) { \
			return (ResourceManager:: TNAME ## Wrapper(&it->second)); \
		} \
		T* dataPtr = LOAD_FUNCTION; \
		ResourceManager:: TNAME ## Entry dataEnt; \
		if (dataPtr!= NULL) { \
			dataEnt.data = dataPtr; \
			dataEnt.key = KEY_USED; \
			dataEnt.userCount = 0; \
			it = ResourceManager:: TNAME ## s.insert(std::pair<std::string, ResourceManager:: TNAME ## Entry>(KEY_USED,dataEnt)).first; \
		} else { \
			dataEnt.data = NULL; \
		} \
		return (ResourceManager:: TNAME ## Wrapper(&it->second)); \
	}
