/// Defines: Singleton class to store & retrieve match settings
class Settings
{
public:
    Settings(const Settings&) = delete; // Blocks singleton copy creation

    static Settings& Get()  // Singleton instance access method
    {
        return match_settings;
    }

    void loadSettings(const char *filename);
    int &core_size() const,
        &max_rounds() const,
        &max_cycles() const,
        &max_processes() const,
        &max_warrior_len() const,
        &min_separation() const;

private:
    int core_size, max_rounds, max_cycles, 
        max_processes, max_warrior_len, min_separation;

    // Singleton instance
    static Settings match_settings;
    Settings();
};

// Defines singleton class instance
Settings Settings::match_settings;

void Settings::loadSettings(const char *filename) 
{
    
}