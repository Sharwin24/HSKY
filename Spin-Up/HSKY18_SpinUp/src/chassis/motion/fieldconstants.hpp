#pragma once

namespace src::Motion {

enum class StartingPosition { // Figure out all possible starting positions and name them
    RED_FRONT,
    RED_BACK,
    BLUE_FRONT,
    BLUE_BACK,
};

class FieldConstants {
  public:
    FieldConstants(){};
    ~FieldConstants(){};

    void setStartingPosition(StartingPosition position);
    float getTileSize() const { return 24.0f; }   // [in]
    float getFieldSize() const { return 144.0f; } // [in]
    float getRedGoalPositionX() const { return this->redGoalPositionX; }
    float getRedGoalPositionY() const { return this->redGoalPositionY; }
    float getBlueGoalPositionX() const { return this->blueGoalPositionX; }
    float getBlueGoalPositionY() const { return this->blueGoalPositionY; }

  private:
    StartingPosition startingPosition;
    void setRedGoalPosition(float x, float y) {
        this->redGoalPositionX = x;
        this->redGoalPositionY = y;
    }
    void setBlueGoalPosition(float x, float y) {
        this->blueGoalPositionX = x;
        this->blueGoalPositionY = y;
    }
    float redGoalPositionX;
    float redGoalPositionY;
    float blueGoalPositionX;
    float blueGoalPositionY;
};
} // namespace src::Motion