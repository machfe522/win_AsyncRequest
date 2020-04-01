
#include "message.hpp"
#include "actor.hpp"
#include "mailbox.hpp"
#include "thread_pool.hpp"
#include "established_actor.hpp"

#include  "http_file_source.hpp"
#include "run_loop.hpp"
#include "thread.hpp"

#include "curl/curl.h"
 
class ABase
{
public:

	ABase(int xx)
	{
		printf("num == %d\n", xx);
	}
 

	void Test(const std::string& strUrl)
	{
		
		mbgl::Callback callback;
		request = httpFileSource.request(strUrl, callback);
	}

private:
	mbgl::HTTPFileSource httpFileSource;
	std::unique_ptr<mbgl::AsyncRequest> request;

};
 
int  main()
{
	std::string strUrl = "https://api.mapbox.com/v4/mapbox.mapbox-streets-v8,mapbox.mapbox-terrain-v2.json?access_token=pk.eyJ1IjoibWFtYWNoZmUiLCJhIjoiY2p1dzBndXRjMDdjcDN5cGI2d2duM3d2byJ9.BPbVcece6cQABmqi0CvgZQ&secure";

	 mbgl::Thread<ABase> base("machfe", 8);
	 base.actor().invoke(&ABase::Test, strUrl);
 

	system("pause");

 	return 0;
}
 
