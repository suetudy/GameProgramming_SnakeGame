#include <iostream>
#include <cstdlib>
#include <ctime>
#include <SDL.h>
#include <SDL_image.h>


//랜덤 숫자 생성
int randomNumber(int nr_min, int nr_max)
{
    static int temp = 0;
    static bool initialized = false;
    if (!initialized)
    {
        initialized = true;
        srand(time(NULL));
    }
    temp = rand() % nr_max + nr_min;
    temp -= (temp % 25);
    return temp;
}


//뱀 노드
struct Node {
    SDL_Rect pos;
    bool isNew;
    Node* next;
};

Node* head = nullptr;
Node* iterator = nullptr;
SDL_Renderer* renderer;
char rotation;
bool collision = false;
bool eat = false;
int random1, random2, count;

void addNode() {
    Node* newNode = new Node;
    iterator = head;
    while (iterator->next != nullptr) {
        iterator = iterator->next;
    }
    iterator->next = newNode;
    newNode->isNew = true;
    newNode->pos.x = iterator->pos.x;
    newNode->pos.y = iterator->pos.y;
    newNode->pos.w = 25;
    newNode->pos.h = 25;
    newNode->next = nullptr;
}

void update(Node* head, Node* prev) {
    Node* local_iterator;
    local_iterator = head;
    if (local_iterator->next != nullptr) {
        update(local_iterator->next, local_iterator);
    }
    if (local_iterator->isNew == true) {
        local_iterator->isNew = false;
    }
    else {
        local_iterator->pos.x = prev->pos.x;
        local_iterator->pos.y = prev->pos.y;
    }
}

void checkEat()
{
    if (head->pos.x == random1 && head->pos.y == random2) {
        eat = true;
    }
}

void checkCollision(int width, int height)
{
    if (head->pos.x-25 < 0 || head->pos.x+25 >= width || head->pos.y-25 < 0 || head->pos.y+25 >= height) {
        collision = true;
        std::cout << "\nGAME OVER\nSCORE: " << count << std::endl;
    }
    else {
        iterator = head->next;
        while (iterator != nullptr)
        {
            if (head->pos.x == iterator->pos.x && head->pos.y == iterator->pos.y)
            {
                collision = true;
                std::cout << "\nGAME OVER\nSCORE: " << count << std::endl;
                break;
            }
            else
            {
                iterator = iterator->next;
            }
        }
    }
}
// Game loop
bool running = true;
bool gameOver = false;
bool gameStarted = false;
bool ready = true;
void move(int width, int height) {
    
    //충돌시 더 이상 이동하지 않고 함수 종료
    if (collision) {
        return; 
    }
    update(head, head);
    
    if (gameStarted) {
        switch (rotation)
        {
        case 'u':
            head->pos.y -= 25;
            break;
        case 'd':
            head->pos.y += 25;
            break;
        case 'l':
            head->pos.x -= 25;
            break;
        case 'r':
            head->pos.x += 25;
            break;
        default:
            break;
        }
    }
    


    checkCollision(1000, 700);
}

int main(int argc, char** argv) {

    int delayms = 100;
    int mindelay = 40;
    count = 0;
    random1 = randomNumber(0, 1000);
    random2 = randomNumber(0, 700);

    SDL_Rect startposition = {200,200,25,25};
    SDL_Rect eatposition = {random1,random2,25,25};
    head = new Node;
    head->isNew = false;
    head->pos = startposition;
    head->next = nullptr;

    rotation = 'p';

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cout << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }


    // Create a window
    SDL_Window* window = SDL_CreateWindow("Snake", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1000, 700, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cout << "Failed to create SDL window: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Create a renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
        std::cout << "Failed to create SDL renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Initialize SDL_image
    if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
        std::cout << "Failed to initialize SDL_image: " << IMG_GetError() << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Load images
    SDL_Surface* headSurface = IMG_Load("../Resources/IMG_3041.PNG");
    if (headSurface == nullptr) {
        std::cout << "Failed to load head imageh: " << IMG_GetError() << std::endl;
        IMG_Quit();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_Texture* headTexture = SDL_CreateTextureFromSurface(renderer, headSurface);
    SDL_FreeSurface(headSurface);

    SDL_Surface* bodySurface = IMG_Load("../Resources/IMG_3042.PNG");
    if (bodySurface == nullptr) {
        std::cout << "Failed to load body image: " << IMG_GetError() << std::endl;
        IMG_Quit();
        SDL_DestroyTexture(headTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_Texture* bodyTexture = SDL_CreateTextureFromSurface(renderer, bodySurface);
    SDL_FreeSurface(bodySurface);

    SDL_Surface* itemSurface = IMG_Load("../Resources/IMG_3040.PNG");
    if (itemSurface == nullptr) {
        std::cout << "Failed to load item image: " << IMG_GetError() << std::endl;
        IMG_Quit();
        SDL_DestroyTexture(bodyTexture);
        SDL_DestroyTexture(headTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_Texture* itemTexture = SDL_CreateTextureFromSurface(renderer, itemSurface);
    SDL_FreeSurface(itemSurface);

    SDL_Surface* backgroundSurface = IMG_Load("../Resources/IMG_3025.JPG");
    if (backgroundSurface == nullptr) {
        std::cout << "Failed to load background image: " << IMG_GetError() << std::endl;
        IMG_Quit();
        SDL_DestroyTexture(itemTexture);
        SDL_DestroyTexture(bodyTexture);
        SDL_DestroyTexture(headTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);

    SDL_Surface* readySurface = IMG_Load("../Resources/Ready.PNG");
    if (readySurface == nullptr) {
        std::cout << "Failed to load ready image: " << IMG_GetError() << std::endl;
        IMG_Quit();

        SDL_DestroyTexture(itemTexture);
        SDL_DestroyTexture(bodyTexture);
        SDL_DestroyTexture(headTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_Texture* readyTexture = SDL_CreateTextureFromSurface(renderer, readySurface);

    SDL_Surface* gameOverSurface = IMG_Load("../Resources/GameOver.PNG");
    if (gameOverSurface == nullptr) {
        std::cout << "Failed to load game over image: " << IMG_GetError() << std::endl;
        IMG_Quit();
        SDL_DestroyTexture(readyTexture);
        SDL_DestroyTexture(backgroundTexture);
        SDL_DestroyTexture(itemTexture);
        SDL_DestroyTexture(bodyTexture);
        SDL_DestroyTexture(headTexture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    SDL_Texture* gameOverTexture = SDL_CreateTextureFromSurface(renderer, gameOverSurface);
    SDL_FreeSurface(backgroundSurface);

    
    SDL_Event event;
    while (running) {
        SDL_Delay(delayms);
        checkEat();
        move(1000, 700);
        // Event listener
        while (SDL_PollEvent(&event)) {
            // Quit event
            if (event.type == SDL_QUIT) {
                running = false;
            }
            // Mouse event
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                // Left mouse button
                if (event.button.button == SDL_BUTTON_LEFT) {
                    if (gameOver) {
                        collision = false;
                        count = 0;
                        delayms = 100;
                        random1 = randomNumber(0, 1000);
                        random2 = randomNumber(0, 700);

                        // Reset  position
                        eatposition.x = random1;
                        eatposition.y = random2;
                        if (delayms > mindelay) {
                            delayms -= 2;
                        }

                        // Reset snake position
                        head->pos.x = 200;
                        head->pos.y = 200;
                        head->next = nullptr;

                        // Remove existing snake nodes
                        Node* current = head->next;
                        while (current != nullptr) {
                            Node* next = current->next;
                            delete current;
                            current = next;
                        }

                        // Hide game over image
                        gameOver = false;
                        ready = true;
                       
                    }
                }
            }
            // Keyboard event
            if (event.type == SDL_KEYDOWN) {
                
                switch (event.key.keysym.sym) {
                case SDLK_ESCAPE:
                    running = false;
                    break;
                case SDLK_UP:
                    if (rotation != 'd') {
                        rotation = 'u';
                    }
                    if (ready  && !gameStarted) {
                        gameStarted = true;
                        rotation = 'u';
                    }
                    break;
                case SDLK_DOWN:
                    if (rotation != 'u') {
                        rotation = 'd';
                    }
                    if (ready  && !gameStarted) {
                        gameStarted = true;
                        rotation = 'd';
                    }
                    break;
                case SDLK_LEFT:
                    if (rotation != 'r') {
                        rotation = 'l';
                    }
                    if (ready && !gameStarted) {
                        gameStarted = true;
                        rotation = 'l';
                    }
                    break;
                case SDLK_RIGHT:
                    if (rotation != 'l') {
                        rotation = 'r';
                    }
                    if (ready  && !gameStarted) {
                        gameStarted = true;
                        rotation = 'r';
                    }
                    break;
                case SDLK_x:
                    addNode();
                    break;
                case SDLK_p:
                    // Pause
                    break;
                default:
           
                    break;
                }
            }
        }

    
        if (collision == true) {
            gameOver = true;
            gameStarted = false;
            ready = false;
        }

        
        if (eat == true) {
            addNode();
            count++;
            eat = false;
            random1 = randomNumber(0, 1000);
            random2 = randomNumber(0, 700);
            eatposition.x = random1;
            eatposition.y = random2;
            if (delayms > mindelay) {
                delayms -= 2;
            }
        }

        
        // Draw background
        SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);

        // Draw item
        SDL_RenderCopy(renderer, itemTexture, nullptr, &eatposition);

        // Draw snake
        SDL_RenderCopy(renderer, headTexture, nullptr, &head->pos);
        iterator = head->next;
        while (iterator != nullptr) {
            SDL_RenderCopy(renderer, bodyTexture, nullptr, &iterator->pos);
            iterator = iterator->next;
        }
        if (ready && !gameStarted) {

            SDL_RenderCopy(renderer, readyTexture, nullptr, nullptr);
        }

        if (gameOver && !gameStarted) {

            SDL_RenderCopy(renderer, gameOverTexture, nullptr, nullptr);
        }


        // Render to the screen
        SDL_RenderPresent(renderer);
    }

    // Cleanup
    delete head;
    SDL_DestroyTexture(readyTexture);
    SDL_DestroyTexture(backgroundTexture);
    SDL_DestroyTexture(itemTexture);
    SDL_DestroyTexture(bodyTexture);
    SDL_DestroyTexture(headTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
