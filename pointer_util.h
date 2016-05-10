#ifndef POINTER_UTIL_H
#define POINTER_UTIL_H

#ifndef SAFE_DELETE
#define SAFE_DELETE(x) if( (x) != NULL) {delete (x); (x) = NULL;}
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(x) if( (x) != NULL) {delete[](x); (x) = NULL;}
#endif

/// 根据指针调用free 接口
#ifndef SAFE_FREE
#define SAFE_FREE(x) if((x)!= NULL) {free(x); (x) = NULL;}
#endif

// 根据指针调用Release 接口
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) if((x) != NULL) {(x)->Release(); (x) =NULL;} 
#endif

#endif