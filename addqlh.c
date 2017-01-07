/* qlh - small utitily which adds header for
   QL Emulator

   Invocation: qlh <executable to be patched>

   2016-12-26, OL
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

int main(int argc, char* argv[])
{
  FILE* fp = NULL;
  uint8_t header[30] = {
    0x5d, 0x21, 0x51, 0x44, 0x4f, 0x53, 0x20, 0x46, 0x69, 0x6c,
    0x65, 0x20, 0x48, 0x65, 0x61, 0x64, 0x65, 0x72, 0x00, 0x0f,
    0x00, 0x01, 0x00, 0x00, 0x02, 0xa8, 0x00, 0x00, 0x00, 0x00 };
  uint8_t* p_data = NULL;
  unsigned long size, bytes_read, bytes_written;
  int i;

  if( argc < 2 || !strcmp( argv[1], "--help" ) ) {
    printf("Invocation: qlh <exec-file to be patched>\n\n");
    return -1;
  }

  fp=fopen(argv[1],"rb");
  if( fp == NULL ) {
    fprintf( stderr, "Could not open file %s error!\n", argv[1] );
    return -1;
  }

  fseek(fp, 0L, SEEK_END); size = ftell(fp);
  fseek(fp, 0L, SEEK_SET);

  p_data = malloc( size+sizeof(header) );
  if( p_data == NULL ) {
    fprintf( stderr, "%s, %d: out of memory error!\n", __FILE__, __LINE__ );
    return -1;
  }

  bytes_read = fread( p_data + sizeof(header), sizeof(uint8_t), size, fp );
  fclose( fp );
  if( bytes_read != size) {
    fprintf( stderr, "%s, %d: error while reading file %s!\n",
             __FILE__, __LINE__, argv[1] );
    return -1;
  }

  if( ! memcmp( p_data + sizeof(header), header, sizeof(header) ) )
  {
    fprintf( stderr, "header already present, no patch needs to be applied!\n");
    return -1;
  }

  memcpy( p_data, header, sizeof(header ) );

  fp=fopen(argv[1],"wb");
  if( fp == NULL ) {
    fprintf( stderr, "Could not open file %s error!\n", argv[1] );
    return -1;
  }

  bytes_written = fwrite( p_data, sizeof(uint8_t), size+sizeof(header), fp );
  if( bytes_written != size+sizeof(header) )
  {
    fprintf( stderr, "%s, %d: write error occured!\n", __FILE__, __LINE__ );
    return -1;
  }
  fclose( fp );

  return 0;
}
