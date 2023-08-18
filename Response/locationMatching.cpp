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


void locationMatching(std::string url, Client &client)
{
    std::vector<location> locations = client.getServer().getLocation();

    sort(locations.begin(), locations.end(), locSort);

    std::cout << "Size: " << locations.size() << std::endl;
    while (url.length())
    {
        for (size_t i = 0; i < locations.size(); i++)
        {
            if (locations[i].getPath() == url || locations[i].getPath() == url + "/")
            {
                client.setlocation(locations[i]);
                return;
            }
        }
        if (url.length() == 1)
        {
            for (size_t i = 0; i < locations.size(); i++)
            {
                // std::cout << "location: " << locations[i]._path << std::endl;
                // std::cout << "Uri: " << url << std::endl << std::endl;
                if (locations[i].getPath() == "/")
                {
                    // std::cout << "location found: " << locations[i].getPath() << std::endl;
                    client.setlocation(locations[i]);
                    break;
                }
            }
            // std::cout << "Not Found \n";
            client.setStatus(404);
            return;
        }
        subUrl(url);
    }
}