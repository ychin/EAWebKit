Directory layout explanation - 

* \ea contains all code EAWebKit related. It is small case (ea) instead of upper case(EA) just to be in sync with other platforms.
* \ea\Api contains the code which provides an interface to all WebCore details.
* \ea\WebCoreSupport implements functionality to support WebCore clients.

* \ea\Api\EAWebKit is the code that is exposed to the user application with the exception of \internal folder. This is, as 
directory name suggests, the API that EAWebKit exposes/implements. It also defines the interfaces to cross-platform systems (such as FileSystem)
that the library integrator is supposed to implement.
* \ea\Api\private implements functionality to support EAWebKit API and has implementation similar to other popular ports of WebKit. 
The reason to break API in two separate directories is that EAWebKit exposes a limited subset of functionality to the user application. 