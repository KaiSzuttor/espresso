/*
  Copyright (C) 2010,2011,2012,2013,2014,2015,2016 The ESPResSo project
  Copyright (C) 2002,2003,2004,2005,2006,2007,2008,2009,2010
    Max-Planck-Institute for Polymer Research, Theory Group

  This file is part of ESPResSo.

  ESPResSo is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  ESPResSo is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef ESPRESSO_H5MD_CORE_HPP
#define ESPRESSO_H5MD_CORE_HPP
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <unordered_map>
#include <mpi.h>
#include <boost/filesystem.hpp>
#include "cells.hpp"
#include "global.hpp"
#include <h5xx/h5xx.hpp>


extern double sim_time;
extern double time_step;
extern double box_l[3];


namespace writer {
namespace h5md {


typedef boost::multi_array<double,3> double_array_3d;
typedef boost::multi_array<int,3> int_array_3d;

/**
 * @brief Class for writing H5MD files.
**/
class File
{
    public:
    /**
     * @brief Constructor of the "File" class.
     * @param filename Path to the .h5 file.
     * @param python_script_path Path to the python simulation script.
     */
    File(std::string const &filename, std::string const &script_name);
    /*
     * @brief Method to perform the renaming of the temporary file from
     * "filename" + "_tmp" to "filename" and close the file before renaming.
     */
    void Close();

    enum WriteData { W_POS  = 1 << 0,
                     W_V    = 1 << 1,
                     W_F    = 1 << 2,
                     W_TYPE = 1 << 3,
                     W_MASS = 1 << 4 };
    /**
     * @brief General method to write to the datasets which calls more specific write methods.
     * @param Boolean values for position, velocity, force and mass.
     */
    void Write(int write_dat);

    /**
     * @brief Method to write the energy contributions to the H5MD file.
     * @param Boolean values for total, kinetic.
     * \todo Implement this method.
     */
    void WriteEnergy(bool total = true, bool kinetic = true);


    private:
    bool check_file_exists(const std::string &name)
    {
        std::ifstream f(name.c_str());
        return f.good();
    };
    /**
     * @brief Method to check if the H5MD structure is present in the file.
     * Only call this on valid HDF5 files.
     * @param filename The Name of the hdf5-file to check.
     * @return TRUE if H5MD structure is present, FALSE else.
     */
    bool check_for_H5MD_structure(std::string const &filename);
    /**
     * @brief Method that performs all the low-level stuff for writing the particle
     * positions to the dataset.
     */
    template <typename T>
    void WriteDataset(T &data, const std::string& path);
    /*
     * @brief Method to write the simulation script to the dataset.
     */
    void WriteScript(std::string const &filename);

    /**
     * @brief Creates a new H5MD file.
     * @param filename The filename
     */
    void createNewFile(const std::string& filename);

    /**
     * @brief Loads an existing H5MD file.
     * @param filename The filename
     */
    void loadFile(const std::string& filename);

    /**
     * @brief Initializes the necessary data to create the groups and datsets.
     */
    void init_filestructure();

    /**
     * @brief Creates the necessary HDF5 datasets.
     * @param only_load Set this to true if you want to append to an existing file.
     */
    void createDatasets(bool only_load);

    /**
     * @brief Creates the necessary HDF5 groups.
     */
    void createGroups();

    /**
     * Because we change the name of the file automatically, its a member variable.
     */
    std::string user_filename;
    std::string backup_filename;
    boost::filesystem::path absolute_script_path;
    h5xx::file h5md_file;

    struct DatasetDescriptor  {
        std::string path;
        hsize_t dim;
        hsize_t size;
        h5xx::datatype type;
    };
    std::vector<std::string> group_names;
    std::vector<DatasetDescriptor> dataset_descriptors;
    std::unordered_map<std::string, h5xx::group> groups;
    std::unordered_map<std::string, h5xx::dataset> datasets;
};


struct incompatible_h5mdfile : public std::exception {
    const char* what () const throw ()
    {
        return "The given hdf5 file does not have a valid h5md structure!";
    }
};


} /* namespace h5md */
} /* namespace writer */
#endif /* ESPRESSO_H5MD_CORE_HPP */
