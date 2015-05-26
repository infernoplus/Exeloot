/*
 * import.hpp
 *
 *  Created on: May 25, 2015
 *      Author: inferno
 */

#ifndef IMPORT_HPP_
#define IMPORT_HPP_

#include <iostream>
#include <algorithm>
#include <cstdlib>
#include <sstream>
#include <fstream>

#include <string.h>
#include <math.h>

namespace model {

class Vert3f {
public:
  float x, y, z;
  Vert3f(float x, float y, float z) {
    this->x=x; this->y=y; this->z=z;
  }
  Vert3f(float x, float y) {
    this->x=x; this->y=y; this->z=0.0f;
  }
  Vert3f() {
    this->x=0.0f; this->y=0.0f; this->z=0.0f;
  }
};

class Triangle {
public:
  uint a, b, c; //Positions
  uint i, j, k; //Normals
  uint u, v, w; //Tex Coords
  uint sg;         //Smoothing Group
  std::string shader;  //Shader Name
  Triangle(uint a, uint b, uint c, uint i, uint j, uint k, uint u, uint v, uint w, uint sg, std::string shader) {
    this->a=a; this->b=b; this->c=c;
    this->i=i; this->j=j; this->k=k;
    this->u=u; this->v=v; this->w=w;
    this->sg=sg;
    this->shader=shader;
  }
  Triangle() { }
};

class Uid {
public:
  uint a, b, c;
  Uid(uint a, uint b, uint c) {
    this->a=a; this->b=b; this->c=c;
  }
  Uid() { }
  bool operator==(const Uid& u) {
    return a==u.a && b==u.b && c==u.c;
  }
};

std::string readFile(const std::string path) {
    std::ifstream t(path);
    std::stringstream buffer;

    buffer << t.rdbuf();
    return buffer.str();
}

char* geometryToBytes(float* data, uint dSize, uint* indices, uint iSize, const std::string shader, uint& oSize) {
  const char* shad = shader.c_str();
  uint sSize = strlen(shad);

  oSize = (dSize * sizeof(float)) + (iSize * sizeof(uint)) + sSize + (3 * sizeof(uint));
  char* out = new char[oSize];
  uint oAt = 0;

  union { char bytes[sizeof(float)]; float val; } floatToByte;
  union { char bytes[sizeof(uint)]; uint val; } uintToByte;
  //Write shader name
  uintToByte.val = sSize;
  for(uint i=0;i<sizeof(uint);i++)
    out[oAt++] = uintToByte.bytes[i];
  for(uint i=0;i<sSize;i++)
    out[oAt++] = shad[i];
  //Write vertex data
  uintToByte.val = dSize;
  for(uint i=0;i<sizeof(uint);i++)
          out[oAt++] = uintToByte.bytes[i];
  for(uint i=0;i<dSize;i++) {
      floatToByte.val = data[i];
      for(uint j=0;j<sizeof(float);j++) {
          out[oAt++] = floatToByte.bytes[j];
      }
  }
  //Write indices
  uintToByte.val = iSize;
  for(uint i=0;i<sizeof(uint);i++)
    out[oAt++] = uintToByte.bytes[i];
  for(uint i=0;i<iSize;i++) {
    uintToByte.val = indices[i];
    for(uint j=0;j<sizeof(uint);j++) {
      out[oAt++] = uintToByte.bytes[j];
    }
  }
  return out;
}

std::string triminate(const std::string data) {
  std::stringstream ss;
  for(uint i=0;i<data.length()-1;i++) {
    char c = data.at(i);
    if(!(c == ' ' && data.at(i+1) == ' '))
      ss << c;
  }
  return ss.str();
}

std::string* splitify(uint& size, const std::string data, const char key) {
  size = 1;
  for(uint i=0;i<data.length();i++)
    if(data.at(i) == key)
      size++;

  std::string* lines = new std::string[size];
  std::stringstream ss;
  int j=0;
  for(uint i=0;i<data.length();i++) {
    char c = data.at(i);
    if(c == key) {
      lines[j++]=ss.str();
      ss.str(std::string());
    }
    else {
      ss << c;
    }
  }
  lines[j]=ss.str();
  return lines;
}

//Return first non-whitespace character
char* getFirst(const std::string data, uint& size) {
  long start=-1; long end=-1;
  for(uint i=0;i<data.length()&&end==-1;i++) {
    if(data.at(i) != ' ' && start == -1)
      start = i;
    else if(data.at(i) == ' ' && start != -1 && end == -1)
      end = i;
  }
  size = end-start;
  char* res = new char[size];
  for(int i=start;i<end;i++)
    res[i-start]=data.at(i);
  return res;
}

//Lol...
bool compare(char* c, const uint size, const std::string d) {
  if(size != d.length())
    return false;
  for(uint i=0;i<size&&i<d.length();i++) {
    if(c[i] != d.at(i))
      return false;
  }
  return true;
}

Vert3f *v, *vn, *vt;
Triangle* f;
int sg = 0; std::string shader = "default";

Vert3f parseV(const std::string line) {
  uint size = 0;
  std::string* spl = splitify(size, line, ' ');
  float x = atof(spl[1].c_str()); float y = atof(spl[2].c_str()); float z = atof(spl[3].c_str());
  return Vert3f{x, y, z};
}

Vert3f parseVn(const std::string line) {
  uint size = 0;
  std::string* spl = splitify(size, line, ' ');
  float x = atof(spl[1].c_str()); float y = atof(spl[2].c_str()); float z = atof(spl[3].c_str());
  return Vert3f{x, y, z};
}

Vert3f parseVt(const std::string line) {
  uint size = 0;
  std::string* spl = splitify(size, line, ' ');
  if(size < 4) { //Supports U,V and U,V,W
    float x = atof(spl[1].c_str()); float y = atof(spl[2].c_str());
    return Vert3f{x, y};
  }
  else {
    float x = atof(spl[1].c_str()); float y = atof(spl[2].c_str()); float z = atof(spl[3].c_str());
    return Vert3f{x, y, z};
  }
}

Triangle parseF(const std::string line) {
  uint size = 0;
  std::string* spl = splitify(size, line, ' ');
  uint asize, bsize, csize;
  std::string* a = splitify(asize, spl[1], '/');
  std::string* b = splitify(bsize, spl[2], '/');
  std::string* c = splitify(csize, spl[3], '/');
  return Triangle{
    atoi(a[0].c_str()), atoi(b[0].c_str()), atoi(c[0].c_str()),
    atoi(a[1].c_str()), atoi(b[1].c_str()), atoi(c[1].c_str()),
    atoi(a[2].c_str()), atoi(b[2].c_str()), atoi(c[2].c_str()),
    sg, shader
  };
}

//TODO: I still think this is probably buggy.
std::string parseMtl(const std::string line) {
  char* token = new char[7]{'u','s','e','m','t','l', ' '};
  int start = -1, end = -1;
  int j = 0;
  for(uint i=0;i<line.length();i++) {
    if(start == -1 && line.at(i) == token[j])
      j = j+1 < 7 ? j+1 : j;
    else if(line.at(i) != ' ' && start == -1)
      start = i;
    if(line.at(i) == '\n' || line.at(i) == '\r' || line.at(i) == '\00' || !(i+1 < line.length()))
      end = i;
  }
  if(end-start > 1)
    return line.substr(start, end-start+1);
  return "default";
}

uint vc=0, vnc=0, vtc=0, fc=0, sc=0; //Counts
void parseticulate(std::string* lines, const uint& size) {
  for(uint i=0;i<size;i++) {
    std::string line = lines[i];
    uint dsize = 0;
    char* c = getFirst(line, dsize);
    if(compare(c, dsize, "v"))
      vc++;
    else if(compare(c, dsize, "vn"))
      vnc++;
    else if(compare(c, dsize, "vt"))
      vtc++;
    else if(compare(c, dsize, "f"))
      fc++;
    else if(compare(c, dsize, "usemtl"))
      sc++;
  }

  std::cout << "Building model ...\n";
  std::cout << "Vertexes: " << vc << ", Normals: " << vnc << ", Tex Coords: " << vtc << ", Triangles: " << fc << " ...\n";

  v = new Vert3f[vc];
  vn = new Vert3f[vnc];
  vt = new Vert3f[vtc];
  f = new Triangle[fc];
  vc=0, vnc=0, vtc=0, fc=0;

  for(uint i=0;i<size;i++) {
    std::string line = lines[i];
    uint dsize = 0;
    char* c = getFirst(line, dsize);
    if(compare(c, dsize, "v")) {
      Vert3f res = parseV(line);
      v[vc] = res;
      vc++;
    }
    else if(compare(c, dsize, "vn")) {
      Vert3f res = parseVn(line);
      vn[vnc] = res;
      vnc++;
    }
    else if(compare(c, dsize, "vt")) {
      Vert3f res = parseVt(line);
      vt[vtc] = res;
      vtc++;
    }
    else if(compare(c, dsize, "f")) {
      Triangle res = parseF(line);
      f[fc] = res;
      fc++;
    }
    else if(compare(c, dsize, "s")) {
      sg = 0;
    }
    else if(compare(c, dsize, "usemtl")) {
      shader = parseMtl(line);
    }
  }
}

//If the data for this already exists we return it's index for re-use. Returns -1 if it doesn't exist. Using long since we can't use uint for -1
long dupeSearch(Uid* uids, const uint& size, const Uid& uid) {
  for(long i=0;i<size;i++)
    if(uids[i] == uid)
      return i;
  return -1;
}


char* compressorate(Triangle* t, uint tSize, uint& oSize) {
  uint dSize = (tSize*27); //Reserve maximum amount of possible space (triangle count * 9 vertexes * 3 floats)
  uint iSize = (tSize*3);
  Uid* uids = new Uid[iSize];
  uint* indices = new uint[iSize];
  float* data = new float[dSize];

  uint dAt = 0; uint uAt = 0; uint iAt = 0;
  for(uint i=0;i<tSize;i++) {
    Uid uid = Uid{t[i].a,t[i].i,t[i].u};
    long u = dupeSearch(uids, uAt, uid);
    if(u < 0) {
      uids[uAt] = uid;
      indices[iAt] = uAt; iAt++; uAt++;
      data[dAt]=v[t[i].a-1].x; dAt++; data[dAt]=v[t[i].a-1].y; dAt++; data[dAt]=v[t[i].a-1].z; dAt++;
      data[dAt]=vn[t[i].i-1].x; dAt++; data[dAt]=vn[t[i].i-1].y; dAt++; data[dAt]=vn[t[i].i-1].z; dAt++;
      data[dAt]=vt[t[i].u-1].x; dAt++; data[dAt]=vt[t[i].u-1].y; dAt++; data[dAt]=vt[t[i].u-1].z; dAt++;
    }
    else {
      indices[iAt] = u; iAt++;
    }
    uid = Uid{t[i].b,t[i].j,t[i].v};
    u = dupeSearch(uids, uAt, uid);
    if(u < 0) {
      uids[uAt] = uid;
      indices[iAt] = uAt; iAt++; uAt++;
      data[dAt]=v[t[i].b-1].x; dAt++; data[dAt]=v[t[i].b-1].y; dAt++; data[dAt]=v[t[i].b-1].z; dAt++;
      data[dAt]=vn[t[i].j-1].x; dAt++; data[dAt]=vn[t[i].j-1].y; dAt++; data[dAt]=vn[t[i].j-1].z; dAt++;
      data[dAt]=vt[t[i].v-1].x; dAt++; data[dAt]=vt[t[i].v-1].y; dAt++; data[dAt]=vt[t[i].v-1].z; dAt++;
    }
    else {
      indices[iAt] = u; iAt++;
    }
    uid = Uid{t[i].c,t[i].k,t[i].w};
    u = dupeSearch(uids, uAt, uid);
    if(u < 0) {
      uids[uAt] = uid;
      indices[iAt] = uAt; iAt++; uAt++;
      data[dAt]=v[t[i].c-1].x; dAt++; data[dAt]=v[t[i].c-1].y; dAt++; data[dAt]=v[t[i].c-1].z; dAt++;
      data[dAt]=vn[t[i].k-1].x; dAt++; data[dAt]=vn[t[i].k-1].y; dAt++; data[dAt]=vn[t[i].k-1].z; dAt++;
      data[dAt]=vt[t[i].w-1].x; dAt++; data[dAt]=vt[t[i].w-1].y; dAt++; data[dAt]=vt[t[i].w-1].z; dAt++;
    }
    else {
      indices[iAt] = u; iAt++;
    }
  }

  std::cout << " - Compressed indices: " << iAt << " -> " << iAt - uAt << " ... \n";
  return geometryToBytes(data, dAt, indices, iAt, t[0].shader, oSize);
}

void writeToFile(const std::string out) {
  uint sSize = sc; //Allocate max size, use sAt to determine actual
  std::string* shaders = new std::string[sSize];
  uint sAt = 0;
  for(uint i=0;i<fc;i++) {
    bool found = false;
    for(uint j=0;j<sAt;j++) {
      if(f[i].shader == shaders[j])
        found = true;
    }
    if(!found) {
      shaders[sAt++] = f[i].shader;
    }
  }

  std::cout << "Found " << sAt << " unique geometry groups.\n";
  char** groups = new char*[sAt];
  uint* gSizes = new uint[sAt];
  for(uint i=0;i<sAt;i++) {
    std::cout << "Compressing Geometry Group '" << shaders[i] << "' ... \n";
    uint gAt = 0;
    Triangle* group = new Triangle[fc]; //Allocate maximum size, use gAt to determine actual size;
    for(uint j=0;j<fc;j++) {
      if(f[j].shader == shaders[i])
        group[gAt++] = f[j];
    }
    groups[i] = compressorate(group, gAt, gSizes[i]);
  }

  int k = 0;
  std::ofstream fout(out);
  //Write header
  fout << 'g'; fout << 'l'; fout << 's';
  //Write number of geometry groups
  union { char bytes[sizeof(uint)]; uint val; } uintToByte;
  uintToByte.val = sAt;
  for(uint i=0;i<sizeof(uint);i++)
    fout << uintToByte.bytes[i];
  //Write data
  for(uint i=0;i<sAt;i++)
    for(uint j=0;j<gSizes[i];j++) {
      k++;
      fout << groups[i][j];
    }
  fout.close();

  std::cout << k << " bytes written to '" << out << "' ...\n";
}

int importModel(std::string in, std::string out) {
	  std::cout << "Reading file: '" << in << "' ...\n";
	  std::string data = readFile(in);
	  data = triminate(data);
	  uint size = 0;
	  std::string* lines = splitify(size, data, '\n');

	  std::cout << "Parsing file ...\n";
	  parseticulate(lines, size);

	  std::cout << "Compressing to GLS ... \n";
	  writeToFile(out);
	  std::cout << "Done!\n";

	  return 1;
}

}

#endif /* IMPORT_HPP_ */
