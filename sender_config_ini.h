// Test task from NTR
// CConfigIni class definition for data sender application

#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/filesystem.hpp>
#include <string>
#include <iostream>

class CConfigIni
{
public:
    CConfigIni(std::string fileName) : m_strFileName(fileName)
    {
    }
    ~CConfigIni() = default;

    bool Init()
    {
        if (!boost::filesystem::exists(this->m_strFileName))
        {
            std::cout << "file not exists!\n";
            return false;
        }

        boost::property_tree::ini_parser::read_ini(this->m_strFileName, this->m_root_node);
        return true;
    }

    template <typename T>
    bool GetValue(std::string key, T &value)
    {
        try
        {
            value = this->m_root_node.get<T>(key);
        }
        catch (std::exception e)
        {
            std::cerr << e.what() << std::endl;
            return false;
        }
        return true;
    }

    bool UpdateItem(std::string strRoot_child_name, std::string value)
    {
        this->m_root_node.put<std::string>(strRoot_child_name, value);
        boost::property_tree::ini_parser::write_ini(m_strFileName, m_root_node);
        return true;
    }

private:
    std::string m_strFileName;
    boost::property_tree::ptree m_root_node;
};

constexpr auto min_port_val = 5000;

struct SConfigV
{
    std::string local_ip = "";
    int local_port = 0;
    std::string remote_ip = "";
    int remote_port = 0;
    int total_number_packages = 0;
    int do_delay_after_aliquot = 0;
    int delay_after_aliquot_ms = 0;
    int sent_delay = 0;
    bool write_file = 0;
    bool write_hex = 0;

    bool incorrect = false;

    SConfigV(CConfigIni &config)
    {
        if (config.GetValue("System.local_ip", local_ip) == false)
        {
            std::cout << "Incorrect: System.local_ip\n";
            incorrect = true;
        }
        else if (config.GetValue("System.local_port", local_port) == false)
        {
            std::cout << "Incorrect: System.local_ip\n";
            incorrect = true;
        }
        else if (config.GetValue("System.remote_ip", remote_ip) == false)
        {
            std::cout << "Incorrect: System.local_ip\n";
            incorrect = true;
        }
        else if (config.GetValue("System.remote_port", remote_port) == false)
        {
            std::cout << "Incorrect: System.local_ip\n";
            incorrect = true;
        }
        else if (config.GetValue("Mode.total_number_packages", total_number_packages) == false)
        {
            std::cout << "Incorrect: Mode.total_number_packages\n";
            incorrect = true;
        }
        else if (config.GetValue("Mode.do_delay_after_aliquot", do_delay_after_aliquot) == false)
        {
            std::cout << "Incorrect: Mode.do_delay_after_aliquot\n";
            incorrect = true;
        }
        else if (config.GetValue("Mode.delay_after_aliquot_ms", delay_after_aliquot_ms) == false)
        {
            std::cout << "Incorrect: Mode.delay_after_aliquot_ms\n";
            incorrect = true;
        }
        else if (config.GetValue("Mode.sent_delay", sent_delay) == false)
        {
            std::cout << "Incorrect: Mode.sent_delay\n";
            incorrect = true;
        }
        else if (config.GetValue("Log.write_file", write_file) == false)
        {
            std::cout << "Incorrect: Log.write_file\n";
            incorrect = true;
        }
        else if (config.GetValue("Log.write_hex", write_hex) == false)
        {
            std::cout << "Incorrect: Log.write_hex\n";
            incorrect = true;
        }
        else if (local_ip == "" || (local_port < min_port_val) ||
                 remote_ip == "" || (remote_port < min_port_val))
        {
            std::cout << "Incorrect: local_ip or local_port or remote_ip or remote_port\n";
            incorrect = true;
        }

        if (incorrect)
        {
            std::cout << "Incorrect configuration! Please, check config ini file.\n";            
        }
    }
};
