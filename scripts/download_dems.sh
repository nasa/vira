#!/bin/bash

# Check that user provided output directory
if [ -z "$1" ]; then
    echo "Error: Output directory is required."
    echo "Usage: $0 <output_directory>"
    echo "Example: $0 /home/user/data_sets/"
    exit 1
fi

BASE_DIR="${1%/}"
OUTPUT_DIR="$BASE_DIR/dems"
mkdir -p "$OUTPUT_DIR"


echo "Downloading North Pole Maps"
curl -L https://imbrium.mit.edu/DATA/LOLA_GDR/POLAR/IMG/LDEM_60N_120M.IMG -o "${OUTPUT_DIR}/LDEM_60N_120M.IMG"
curl -L https://imbrium.mit.edu/DATA/LOLA_GDR/POLAR/IMG/LDEM_60N_120M.LBL -o "${OUTPUT_DIR}/LDEM_60N_120M.LBL"


echo ""
echo "Downloading Northern Equatorial Maps"
curl -L https://imbrium.mit.edu/DATA/SLDEM2015/TILES/FLOAT_IMG/SLDEM2015_256_0N_60N_000_120_FLOAT.IMG -o "${OUTPUT_DIR}/SLDEM2015_256_0N_60N_000_120_FLOAT.IMG"
curl -L https://imbrium.mit.edu/DATA/SLDEM2015/TILES/FLOAT_IMG/SLDEM2015_256_0N_60N_000_120_FLOAT.LBL -o "${OUTPUT_DIR}/SLDEM2015_256_0N_60N_000_120_FLOAT.LBL"
curl -L https://imbrium.mit.edu/DATA/SLDEM2015/TILES/FLOAT_IMG/SLDEM2015_256_0N_60N_120_240_FLOAT.IMG -o "${OUTPUT_DIR}/SLDEM2015_256_0N_60N_120_240_FLOAT.IMG"
curl -L https://imbrium.mit.edu/DATA/SLDEM2015/TILES/FLOAT_IMG/SLDEM2015_256_0N_60N_120_240_FLOAT.LBL -o "${OUTPUT_DIR}/SLDEM2015_256_0N_60N_120_240_FLOAT.LBL"
curl -L https://imbrium.mit.edu/DATA/SLDEM2015/TILES/FLOAT_IMG/SLDEM2015_256_0N_60N_240_360_FLOAT.IMG -o "${OUTPUT_DIR}/SLDEM2015_256_0N_60N_240_360_FLOAT.IMG"
curl -L https://imbrium.mit.edu/DATA/SLDEM2015/TILES/FLOAT_IMG/SLDEM2015_256_0N_60N_240_360_FLOAT.LBL -o "${OUTPUT_DIR}/SLDEM2015_256_0N_60N_240_360_FLOAT.LBL"


echo ""
echo "Downloading Southern Equatorial Maps"
curl -L https://imbrium.mit.edu/DATA/SLDEM2015/TILES/FLOAT_IMG/SLDEM2015_256_60S_0S_000_120_FLOAT.IMG -o "${OUTPUT_DIR}/SLDEM2015_256_60S_0S_000_120_FLOAT.IMG"
curl -L https://imbrium.mit.edu/DATA/SLDEM2015/TILES/FLOAT_IMG/SLDEM2015_256_60S_0S_000_120_FLOAT.LBL -o "${OUTPUT_DIR}/SLDEM2015_256_60S_0S_000_120_FLOAT.LBL"
curl -L https://imbrium.mit.edu/DATA/SLDEM2015/TILES/FLOAT_IMG/SLDEM2015_256_60S_0S_120_240_FLOAT.IMG -o "${OUTPUT_DIR}/SLDEM2015_256_60S_0S_120_240_FLOAT.IMG"
curl -L https://imbrium.mit.edu/DATA/SLDEM2015/TILES/FLOAT_IMG/SLDEM2015_256_60S_0S_120_240_FLOAT.LBL -o "${OUTPUT_DIR}/SLDEM2015_256_60S_0S_120_240_FLOAT.LBL"
curl -L https://imbrium.mit.edu/DATA/SLDEM2015/TILES/FLOAT_IMG/SLDEM2015_256_60S_0S_240_360_FLOAT.IMG -o "${OUTPUT_DIR}/SLDEM2015_256_60S_0S_240_360_FLOAT.IMG"
curl -L https://imbrium.mit.edu/DATA/SLDEM2015/TILES/FLOAT_IMG/SLDEM2015_256_60S_0S_240_360_FLOAT.LBL -o "${OUTPUT_DIR}/SLDEM2015_256_60S_0S_240_360_FLOAT.LBL"


echo ""
echo "Downloading South Pole Maps"
curl -L https://pgda.gsfc.nasa.gov/data/LOLA_20mpp/LDEM_60S_60MPP_ADJ.TIF -o "${OUTPUT_DIR}/LDEM_60S_60M.TIF"
curl -L https://pgda.gsfc.nasa.gov/data/LOLA_20mpp/LDEM_80S_20MPP_ADJ.TIF -o "${OUTPUT_DIR}/LDEM_80S_20M.TIF"
curl -L https://pgda.gsfc.nasa.gov/data/LOLA_5mpp/87S/ldem_87s_5mpp.tif -o "${OUTPUT_DIR}/LDEM_87S_5M.TIF"


echo ""
echo "All files downloaded successfully to: $OUTPUT_DIR"