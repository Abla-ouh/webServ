#include "../HTTPServer.hpp"

std::string get_resource_type(const char *res, Client &client)
{
    DIR *dir;

    if (!access(res, F_OK))
    {
        if (!access(res, W_OK))
        {
            if (!(dir = opendir(res)))
                return "FILE";
            else
            {
                closedir(dir);
                return "DIRE";
            }
        }
        else
            client.setStatus(403);
    }
    else
        client.setStatus(404);
    return "INVALID";
}

int delete_directory_contents(const std::string &dir_path)
{
    DIR *dir = opendir(dir_path.c_str());
    dirent *entry;

    if (!dir)
    {
        std::cerr << "Error opening directory " << dir_path << std::endl;
        return 1;
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        std::string entry_path = dir_path + "/" + entry->d_name;
        if (entry->d_type == DT_DIR)
        {
            delete_directory_contents(entry_path);
            rmdir(entry_path.c_str());
        }
        else
            unlink(entry_path.c_str());
    }
    closedir(dir);
    return 0;
}

void getIndexFile(Client &client, std::string &index, std::string src)
{
    std::string file;
    std::vector<std::string> indexes;
    indexes = client.getlocation().getIndex();

    if (!indexes.empty())
    {
        for (size_t i = 0; i < indexes.size(); i++)
        {
            file = src + indexes[i];
            std::cout << "FILE: " << file << std::endl;
            if (!access(file.c_str(), F_OK))
            {
                index = file;
                return;
            }
        }
    }
}

void handleDeleteRequest(Client &client, std::string src)
{
    std::string full_path = src;
    std::string tmp = client.getRequest().getURI();
    std::string rcs_type = get_resource_type(full_path.c_str(), client);
    std::string indexFile;
    std::vector<std::string> allowed_methods = client.getlocation().getAllowMethodes();

    if (find(allowed_methods.begin(), allowed_methods.end(), "DELETE") == allowed_methods.end())
    {
        client.setStatus(405);
        return;
    }

    if (client.getStatus())
        return;
    if (rcs_type == "FILE")
    {
        if (client.getlocation().isCgi())
        {
            client.setState(WAITING_CGI);
            run_cgi(client, src);
            return;
        }
        // Handle file deletion
        if (unlink(full_path.c_str()) == 0)
            // Successfully deleted the file
            client.setStatus(204);
        // sendResponse(client.getClientSocket());
        else
            // Error while deleting the file
            client.setStatus(500);
        // sendErrorResponse(client.getClientSocket(), "500 Internal Server Error");
    }
    else if (rcs_type == "DIRE")
    {
        std::cout << "Tmp: " + tmp << std::endl;
        if (tmp[tmp.length() - 1] != '/')
        {
            client.setStatus(409);
            return ;
        }
        if (client.getlocation().isCgi())
        {
            std::cout << "Has CGI\n";
            getIndexFile(client, indexFile, src);
            if (!indexFile.empty())
            {
                std::cout << "Has Index file\n";
                client.setState(WAITING_CGI);
                run_cgi(client, indexFile);
            }
            else
                client.setStatus(403);
            return;
        }
        // Handle directory deletion
        if (delete_directory_contents(full_path) == 0 && rmdir(full_path.c_str()) == 0)
            // Successfully deleted the directory
            client.setStatus(204);
        else
            // Error while deleting the directory
            client.setStatus(500);
    }
    // else
    //     // Handle invalid resource type
    //     client.setStatus(400);
}