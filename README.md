# 🔥 Robô Autônomo de Detecção e Combate a Incêndio

Este projeto é um robô autônomo capaz de detectar a direção do fogo, mover-se até ele, mirar e acionar uma bomba d’água para combatê-lo. O robô também possui sistema de segurança como recuo automático para que não avance muito no fogo.

---

## 🚀 Funcionalidades Principais

### 🔥 Detecção de Fogo
- Utiliza **3 sensores de chama** para identificar a direção. E cada sensor corresponde a uma área de detecção
- **Inteligência Artificial** para que detecte um foco de fogo onde veja várias chamas e escolha uma principal para apagar e em seguida as outras

### 🎯 Mira Automática
- O servo calcula a **média das ativações** dos sensores e aponta diretamente para o fogo.

### 💧 Controle da Bomba
- Liga automaticamente ao detectar fogo.
- Mantém ligada enquanto o fogo é detectado.
- Possui **limite de tempo contínuo** para evitar queimar a bomba.

### 🚗 Locomoção Inteligente
- **Velocidade adaptativa** conforme distância do fogo.
- **Recuo automático** quando muito próximo.
- **Curvas suaves** usando PWM independente.
- **Freio suave** para evitar trancos.
- **Inteligência Artificial** para desviar de obstáculos

---

## 🧩 Hardware Utilizado

- ESP32
- ESP32CAM  
- 3 sensores de chama  
- 1 bomba d’água + relé  
- 1 servo motor  
- 4 motores DC + driver L298N (Ponte dupla) 
- Baterias 9V/12V (dependendo da bomba e motores)

---

## 🧪 Como Usar

1. Faça upload do código para o ESP32CAM.
2. Rode o código da IA everifique se está funcionando.
3. Faça upload do código para o ESP32.
4. Monte o circuito conforme o diagrama.
5. Ligue o robô.
6. Teste.
7. Ao detectar fogo:
   - Robô se move até a distância ideal
   - O Servo se move com base a direção
   - Bomba é ativada
8. Robô recua automaticamente se ficar muito perto.

---

## 📁 Estrutura do Projeto

```bash
📦 project-name
│-- 📂 IA/ # Important files related to AI
│ │-- 📄 ESP32CAM.ino # Camera archive
│ │-- 📄 best.pt # Trained AI
│ │-- 📄 inteligence.py # Artificial intelligence algorithm
│ │-- 📄 teste_incendio.mp4 # Fire video for AI testing.
│
│-- 📂 robot/ # Hardware-related files
│ │-- 📄 ESP32.ino # Main file for the mechanical part of the robot.
│
│-- 📂 Schemas/ # Help files
│
│-- 📄 README.md # Main documentation for the project

```
