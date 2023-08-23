#include "../HTTPServer.hpp"

bool locSort(location loc1, location loc2) { return (loc1.getPath() > loc2.getPath()); }

void subUrl(std::string &url)
{
    int pos;

    pos = url.find_last_of("/");
    if (url.length() == 1)
        return;
    if (!pos)
        url = "/";
    else if (url[url.length() - 1] == '/')
        url.erase(pos, 1);
    else
        url = url.substr(0, pos);
}

bool setDefaultLocation(std::vector<location> locations, Client &client)
{
    for (size_t i = 0; i < locations.size(); i++)
    {                
        if (locations[i].getPath() == "/")
        {
            client.setlocation(locations[i]);
            return 1;
        }
    }
    return 0;
}

void locationMatching(std::string url, Client &client)
{
    std::vector<location> locations = client.getServer().getLocation();
    sort(locations.begin(), locations.end(), locSort);

    while (url.length())
    {
        for (size_t i = 0; i < locations.size(); i++)
        {
            if (locations[i].getPath() == url || locations[i].getPath() == url + "/")
            {
                client.setlocation(locations[i]);
                client.getRequest().getURI().erase(0, url.length());
                return;
            }
        }
        if (url.length() == 1)
        {
            if (!setDefaultLocation(locations, client))
            {
                std::cout << "No default location found" << std::endl;
                client.setStatus(404);
                return;
            }
        }
        subUrl(url);
    }
}