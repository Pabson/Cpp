#include "headers/codecfactory.h"
#include "headers/deltautil.h"
#include "headers/codecs.h"


#include <chrono>
#include <fstream>



void measureCompresionParameters(string codecName, vector<uint32_t > mydata, size_t N) {
  using namespace FastPForLib;

  IntegerCODEC &codec = *CODECFactory::getFromName(codecName);
  std::vector<uint32_t> compressed_output(N + 1024);

  auto compressionStartTime1 = std::chrono::high_resolution_clock::now();
  size_t compressedsize = compressed_output.size();
  Delta::delta(mydata.data(), mydata.size());
  codec.encodeArray(mydata.data(), mydata.size(), compressed_output.data(), compressedsize);

  compressed_output.resize(compressedsize);
  compressed_output.shrink_to_fit();
  auto compressionEndTime1 = std::chrono::high_resolution_clock::now();
  auto decompressionStartTime1 = std::chrono::high_resolution_clock::now();

  std::vector<uint32_t> mydataback(N);
  size_t recoveredsize = mydataback.size();

  codec.decodeArray(compressed_output.data(), compressed_output.size(), mydataback.data(), recoveredsize);
  mydataback.resize(recoveredsize);
  Delta::inverseDelta(mydataback.data(), mydataback.size());

  auto decompressionEndTime1 = std::chrono::high_resolution_clock::now();
  std::cout << std::setprecision(3);
  std::cout << codec.name() + " Bits per int: " << 32.0 * (static_cast<double>(compressed_output.size()) / static_cast<double>(mydata.size()));
  std::cout << " Compression time in ns: " << std::chrono::duration_cast<std::chrono::nanoseconds>(compressionEndTime1-compressionStartTime1).count();
  std::cout << " Decompression time in ns: " << std::chrono::duration_cast<std::chrono::nanoseconds>(decompressionEndTime1-decompressionStartTime1).count()<<endl;

}


int main() {

  size_t N = 262144;

  string fileNames[4] = {"/home/pabson/mgr/FastPFor/file1.txt", "/home/pabson/mgr/FastPFor/file4.txt", "/home/pabson/mgr/FastPFor/file5.txt", "/home/pabson/mgr/FastPFor/file6.txt"};
  string codecName[6] = {"fastpfor128", "fastpfor256", "simdfastpfor128", "simdfastpfor256", "BP32", "simdbinarypacking"};
  for (int k = 0; k < 4; k++) {
    std::vector<uint32_t> mydata(N);
    std::ifstream infile(fileNames[k]);
    int line;
    int j = 0;

    while (infile >> line) {
      mydata[j] = line;
      j++;
    }
    for (int i = 0; i < 6; i++){
      std::cout << "Iteration nr+ " << i<<endl;
      measureCompresionParameters(codecName[i], mydata, N);
      std::cout <<endl;
    }
  }
}