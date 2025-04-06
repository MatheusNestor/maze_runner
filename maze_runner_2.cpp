#include <iostream>
#include <fstream>
#include <vector>
#include <thread>
#include <chrono>
#include <atomic>

// Representação do labirinto - Matriz
using Maze = std::vector<std::vector<char>>;

// Estrutura para representar uma posição no labirinto
struct Position {
    int row;
    int col;
};

// Variáveis globais
Maze maze;
int num_rows;
int num_cols;
std::atomic<bool> exit_found(false);

// Função para carregar o labirinto de um arquivo
Position load_maze(const std::string& file_name) {
	// TODO: Implemente esta função seguindo estes passos:
    // 1. Abra o arquivo especificado por file_name usando std::ifstream
    // 2. Leia o número de linhas e colunas do labirinto
    // 3. Redimensione a matriz 'maze' de acordo (use maze.resize())
    // 4. Leia o conteúdo do labirinto do arquivo, caractere por caractere
    // 5. Encontre e retorne a posição inicial ('e')
    // 6. Trate possíveis erros (arquivo não encontrado, formato inválido, etc.)
    // 7. Feche o arquivo após a leitura

    std::ifstream file(file_name);
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo: " << file_name << std::endl;
        exit(1);
    }

    file >> num_rows >> num_cols;
    maze.resize(num_rows, std::vector<char>(num_cols));

    Position initial_pos = {-1, -1};
    for (int row = 0; row < num_rows; ++row) {
        for (int col = 0; col < num_cols; ++col) {
            file >> maze[row][col];
            if (maze[row][col] == 'e') {
                initial_pos = {row, col};
            }
        }
    }

    file.close();

    if (initial_pos.row == -1 || initial_pos.col == -1) {
        std::cerr << "Entrada não encontrada." << std::endl;
        exit(1);
    }

    return initial_pos;
}

// Função para imprimir o labirinto
void print_maze(const Maze& m) {
	// TODO: Implemente esta função
    // 1. Percorra a matriz 'maze' usando um loop aninhado
    // 2. Imprima cada caractere usando std::cout
    // 3. Adicione uma quebra de linha (std::cout << '\n') ao final de cada linha do labirinto

    system("clear"); 
    for (const auto& row : m) {
        for (char cell : row) {
            std::cout << cell;
        }
        std::cout << '\n';
    }
    std::cout << std::flush; 

}

// Função para verificar se uma posição é válida
bool is_valid_position(const Maze& m, int row, int col) {
	// TODO: Implemente esta função
    // 1. Verifique se a posição está dentro dos limites do labirinto
    //    (row >= 0 && row < num_rows && col >= 0 && col < num_cols)
    // 2. Verifique se a posição é um caminho válido (maze[row][col] == 'x')
    // 3. Retorne true se ambas as condições forem verdadeiras, false caso contrário

    return row >= 0 && row < num_rows &&
           col >= 0 && col < num_cols &&
           (m[row][col] == 'x' || m[row][col] == 's');
}

// // Função principal para navegar pelo labirinto - Threads
void walk(Maze m, Position pos) {
	// TODO: Implemente a lógica de navegação aqui
    // 1. Marque a posição atual como visitada (maze[pos.row][pos.col] = '.')
    // 2. Chame print_maze() para mostrar o estado atual do labirinto
    // 3. Adicione um pequeno atraso para visualização:
    //    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    // 4. Verifique se a posição atual é a saída (maze[pos.row][pos.col] == 's')
    //    Se for, retorne true
    // 5. Verifique as posições adjacentes (cima, baixo, esquerda, direita)
    //    Para cada posição adjacente:
    //    a. Se for uma posição válida (use is_valid_position()), adicione-a à pilha valid_positions
    // 6. Enquanto houver posições válidas na pilha (!valid_positions.empty()):
    //    a. Remova a próxima posição da pilha (valid_positions.top() e valid_positions.pop())
    //    b. Chame walk recursivamente para esta posição
    //    c. Se walk retornar true, propague o retorno (retorne true)
    // 7. Se todas as posições foram exploradas sem encontrar a saída, retorne false

    if (exit_found) return;

    if (m[pos.row][pos.col] == 's') {
        m[pos.row][pos.col] = 'o';       
    	print_maze(m); 
                  
    	exit_found = true;
    	return;
    }

    m[pos.row][pos.col] = 'o';

    print_maze(m);  
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    m[pos.row][pos.col] = '.';

    std::vector<Position> directions = {
        {pos.row - 1, pos.col},
        {pos.row + 1, pos.col},
        {pos.row, pos.col - 1},
        {pos.row, pos.col + 1}
    };

    std::vector<std::thread> threads;
    bool first = true;

    for (const auto& next : directions) {
        if (exit_found) break;
        if (is_valid_position(m, next.row, next.col)) {
            if (first) {
                first = false;
                walk(m, next);
            } else {
                threads.emplace_back(walk, m, next);
            }
        }
    }

    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <arquivo_labirinto>" << std::endl;
        return 1;
    }

    Position start = load_maze(argv[1]);
    walk(maze, start);

    if (exit_found)
        std::cout << "Saída encontrada!" << std::endl;
    else
        std::cout << "Não foi possível encontrar a saída." << std::endl;

    return 0;
}

// Nota sobre o uso de std::this_thread::sleep_for:
// 
// A função std::this_thread::sleep_for é parte da biblioteca <thread> do C++11 e posteriores.
// Ela permite que você pause a execução do thread atual por um período especificado.
// 
// Para usar std::this_thread::sleep_for, você precisa:
// 1. Incluir as bibliotecas <thread> e <chrono>
// 2. Usar o namespace std::chrono para as unidades de tempo
// 
// Exemplo de uso:
// std::this_thread::sleep_for(std::chrono::milliseconds(50));
// 
// Isso pausará a execução por 50 milissegundos.
// 
// Você pode ajustar o tempo de pausa conforme necessário para uma melhor visualização
// do processo de exploração do labirinto.