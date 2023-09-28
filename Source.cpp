#include <SFML/Graphics.hpp>
#include <iostream>
#include <cassert>
#include <filesystem>
#include <fstream>
#include <any>


namespace Dev
{
    namespace Config
    {
        struct WindowConfig { int W;  int H; };
        struct FontConfig { std::string N; int S; int R, G, B; };
        struct RectangleConfig { std::string N; float X, Y, SX, SY; int R, G, B; float W, H; };
        struct CircleConfig { std::string N; float X, Y, SX, SY; int R, G, B; float Radius; };

        //If fail returns non-zero, otherwise returns 0
        int init(Config::WindowConfig& windowConfig, Config::FontConfig& fontConfig, std::vector<Config::RectangleConfig>& rectConfig, std::vector<Config::CircleConfig>& circConfig)
        {

            std::filesystem::path filePath{ "C:\\Users\\mailt\\source\\repos\\SFML\\config.txt" };
            
            std::ifstream inFile{ filePath };
            std::string line;

            if (!inFile)
            {
                std::cerr << "Unable to open file: " << filePath << '\n';
                return 1;
            }

            while (inFile)
            {
                inFile >> line;

                if (line == "Window")
                {
                    inFile >> windowConfig.W >> windowConfig.H;
                }

                if (line == "Font")
                {
                    inFile >> fontConfig.N >> fontConfig.S >> fontConfig.R >> fontConfig.G >> fontConfig.B;
                }

                if (line == "Rectangle")
                {
                    RectangleConfig rectConf{};
                    inFile >> rectConf.N >> rectConf.X >> rectConf.Y >> rectConf.SX >> rectConf.SY >> rectConf.R >> rectConf.G >> rectConf.B >> rectConf.W >> rectConf.H;
                    rectConfig.push_back(rectConf);
                }
                if (line == "Circle")
                {
                    CircleConfig circConf{};
                    inFile >> circConf.N >> circConf.X >> circConf.Y >> circConf.SX >> circConf.SY >> circConf.R >> circConf.G >> circConf.B >> circConf.Radius;
                    circConfig.push_back(circConf);
                }

            }
            return 0;
        }
    }

    template<typename T, typename U>
    class Shape
    {
    public:

        Shape(const T& config, const Config::FontConfig& fontConfig, const sf::Font& myFont) :
            m_config{ config }, m_moveSpeed{ m_config.SX, m_config.SY }, m_text{ config.N, myFont, static_cast<unsigned int>(fontConfig.S) }
        {
            cons_helper();
            m_shape.setFillColor(sf::Color(config.R, config.G, config.B));
            m_shape.setPosition(config.X, config.Y);
            setTextToOrigin();
        }

        const U& getShape() const { return m_shape; };
        const sf::Text& getText() const{ return m_text; };

        void setTextToOrigin()
        {
            m_text.setOrigin(m_text.getLocalBounds().left + m_text.getLocalBounds().width / 2.0f,
                m_text.getLocalBounds().top + m_text.getLocalBounds().height / 2.0f);
        }

        //text moves to the center of circle
        void moveText()
        {
            m_text.setPosition(m_shape.getGlobalBounds().getPosition().x + m_shape.getLocalBounds().width / 2.0f,
                m_shape.getGlobalBounds().getPosition().y + m_shape.getLocalBounds().height / 2.0f);
        }

        //move circle(basic animation)
        void moveShape()
        {
            m_shape.setPosition(m_shape.getPosition().x + m_moveSpeed.first, m_shape.getPosition().y + m_moveSpeed.second);
        }

        void checkCollisionWindow(const sf::Vector2u windowSize)
        {
            if (m_shape.getGlobalBounds().getPosition().x < 0)
            {
                m_moveSpeed.first *= -1.0f;
            }
            if (m_shape.getGlobalBounds().getPosition().y < 0)
            {
                m_moveSpeed.second *= -1.0f;
            }
            if (m_shape.getGlobalBounds().getPosition().x + m_shape.getLocalBounds().width >= windowSize.x)
            {
                m_moveSpeed.first *= -1.0f;
            }
            if (m_shape.getGlobalBounds().getPosition().y + m_shape.getLocalBounds().height >= windowSize.y)
            {
                m_moveSpeed.second *= -1.0f;
            }
        }
        
    private:

        T m_config{};
        U m_shape{};
        std::pair<float, float> m_moveSpeed{};
        sf::Text m_text;

        void cons_helper();
    };

    template<>
    void Shape<Config::CircleConfig, sf::CircleShape>::cons_helper()
    {
        m_shape.setRadius(m_config.Radius);
    }

    template<>
    void Shape<Config::RectangleConfig, sf::RectangleShape>::cons_helper()
    {
        m_shape.setSize(sf::Vector2f(m_config.W, m_config.H));
    }

    using Circle = Shape<Config::CircleConfig, sf::CircleShape>;
    using Rectangle = Shape<Config::RectangleConfig, sf::RectangleShape>;

}



int main()
{
    Dev::Config::WindowConfig windowConfig{};
    Dev::Config::FontConfig fontConfig{};
    std::vector <Dev::Config::RectangleConfig> rectConfig;
    std::vector<Dev::Config::CircleConfig> circConfig;

    Dev::Config::init(windowConfig, fontConfig, rectConfig, circConfig);

    // create a new window of size 800 by 600 pixel
    // top-left of the windows is (0,0) abd bottom-right is (w,h)
    int wWidth = windowConfig.W;
    int wHeight = windowConfig.H;
    sf::RenderWindow window(sf::VideoMode(wWidth, wHeight), "SFML works!");
    //let's load a font so we can display some text
    sf::Font myFont;
    
    //checking font loaded or not
    std::string fontFilePath = std::filesystem::current_path().append("tech.ttf").string();
    assert(myFont.loadFromFile(fontFilePath) && "Could not load font!");

    //let's create a container that contains Dev::Circle and Dev::Rectangle objects
    std::vector<std::any> container{};

    container.push_back(std::make_shared<Dev::Circle>(Dev::Circle{ circConfig[0],fontConfig, myFont }));
    container.push_back(std::make_shared<Dev::Circle>(Dev::Circle{ circConfig[1],fontConfig, myFont }));
    container.push_back(std::make_shared<Dev::Circle>(Dev::Circle{ circConfig[2],fontConfig, myFont }));

    container.push_back(std::make_shared<Dev::Rectangle>(Dev::Rectangle{ rectConfig[0],fontConfig,myFont }));
    container.push_back(std::make_shared<Dev::Rectangle>(Dev::Rectangle{ rectConfig[1],fontConfig,myFont }));
    container.push_back(std::make_shared<Dev::Rectangle>(Dev::Rectangle{ rectConfig[2],fontConfig,myFont }));



 
    //main loop - continues for each frame while window is open
    while (window.isOpen())
    {
        //event handling
        sf::Event event;
        while (window.pollEvent(event))
        {
            //this event triggers when the windows is closed
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }


            //this event is triggered when a key is pressed
            if (event.type == sf::Event::KeyPressed)
            {
                window.close();
            }
        }

        for (auto& i : container)
        {
            if (i.type() == typeid(std::shared_ptr<Dev::Circle>))
            {
                auto value = std::any_cast<std::shared_ptr < Dev::Circle>> (i);
                value->checkCollisionWindow(window.getSize());
                value->moveShape();
                value->moveText();
            }
            if (i.type() == typeid(std::shared_ptr<Dev::Rectangle>))
            {
                auto value = std::any_cast<std::shared_ptr<Dev::Rectangle>>(i);
                value->checkCollisionWindow(window.getSize());
                value->moveShape();
                value->moveText();
            }
        }
        //basic rendering function calls

        window.clear(); //clear the windows of anything previously drawn 

        //draw shapes and texts
        for (auto& i : container)
        {
            if (i.type() == typeid(std::shared_ptr<Dev::Circle>))
            {
                auto value = std::any_cast<std::shared_ptr < Dev::Circle>> (i);
                window.draw(value->getShape());
                window.draw(value->getText());
            }
            if (i.type() == typeid(std::shared_ptr<Dev::Rectangle>))
            {
                auto value = std::any_cast<std::shared_ptr<Dev::Rectangle>>(i);
                window.draw(value->getShape());
                window.draw(value->getText());
            }
        }

        window.display();       //call the windows display function
    }
    return 0;
}


