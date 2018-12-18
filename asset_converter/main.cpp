#include <iostream>
#include <string>

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>

#include <filesystem>

namespace fs = std::filesystem;


static const std::vector<std::string> extensions {
    ".jpg", ".jpeg",".png"
};

std::string isValidExtensions(const std::vector<std::string>&   exts,fs::path file) {
    if(!file.has_extension()) {
        printf("Erreur le fichier %s na pas d'extension\n",file.string().c_str());
        return "";
    }
    for(const auto& e : exts) {
        if(const auto s = file.extension(); s == e) {
            return s;
        }
    }
    return "";
}

// Convertit les texture du possier en png
void convert_texture(fs::path file,cv::Size max_size) {
    // Regarde si l'extension est une image
    std::string ext = isValidExtensions(extensions,file);
    if(ext.empty()) {
        printf("Extension non valide %s \n",file.string().c_str());
        return;
    }
    cv::Mat img = cv::imread(file.string());
    if(img.empty()) {
        printf("Fichier n'existe pas %s \n",file.string().c_str());
        return;
    }
    // Regarde si l'image n'a pas 4 channel
    if(int i = img.channels(); i != 4) {
        printf("L'image ne possède pas 4 channel mais %d\n",i);
    }

    file = file.replace_extension(".png");
    if(!cv::imwrite(file.string(),img,{cv::IMWRITE_PNG_COMPRESSION,9})) {
        printf("Echer de l'ecriture du fichier %s\n",file.string().c_str());
        return;
    }

}


int main() {


    convert_texture("/home/wq/models/horloge_bois.png",cv::Size(1024,1024));


    return 0;
}