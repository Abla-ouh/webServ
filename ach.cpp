if (!client.bodyChunked && client.getCurrentState() == BODY_READING)
{
    if (client.firstTime)
    {
        char *holder;
        holder = substr_no_null(data, client.bodyPos, rd, rd);
        
        std::cout << "holder: " << holder << std::endl;
        std::cout << "getFileSize: " << getFileSize(client.file_name) << std::endl;
        
        size_t contentLength = std::atoi(request.getHeader("Content-Length").c_str());
        size_t remainingBytes = contentLength - (rd - client.bodyPos);
        
        if (getFileSize(client.file_name) < contentLength)
        {
            std::cout << "content length: " << contentLength << std::endl;
            std::cout << "file: " << client.file_name << std::endl;
            std::cout << "readBytes: " << rd - client.bodyPos << std::endl;
            std::cout << "getFileSize: " << getFileSize(client.file_name) << std::endl;
            
            write(client.file, holder, rd - client.bodyPos);
            
            if (getFileSize(client.file_name) >= contentLength)
            {
                close(client.file);
                client.isBodyReady = true;
                client.ready = true;
                FD_CLR(client.getClientSocket(), &readSet);
                FD_SET(client.getClientSocket(), &writeSet);
            }
            else
            {
                // Need more data to complete content length
                client.bodyPos = 0; // Reset body position
            }
        }
        else
        {
            // File size is already greater than or equal to content length
            close(client.file);
            client.isBodyReady = true;
            client.ready = true;
            FD_CLR(client.getClientSocket(), &readSet);
            FD_SET(client.getClientSocket(), &writeSet);
        }
        
        delete []holder;
        client.firstTime = 0;
    }
    else
    {
        size_t contentLength = std::atoi(request.getHeader("Content-Length").c_str());
        size_t remainingBytes = contentLength - getFileSize(client.file_name);
        
        if (remainingBytes > 0)
        {
            write(client.file, data, rd);
            
            if (getFileSize(client.file_name) >= contentLength)
            {
                close(client.file);
                client.isBodyReady = true;
                client.ready = true;
                FD_CLR(client.getClientSocket(), &readSet);
                FD_SET(client.getClientSocket(), &writeSet);
            }
        }
        else
        {
            // Content length exceeded, close the file and switch to write set
            close(client.file);
            client.isBodyReady = true;
            client.ready = true;
            FD_CLR(client.getClientSocket(), &readSet);
            FD_SET(client.getClientSocket(), &writeSet);
        }
    }
}
