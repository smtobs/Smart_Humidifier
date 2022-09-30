#ifndef MODEL_HPP
#define MODEL_HPP

class ModelListener;

class Model
{
public:
    Model();

    void bind(ModelListener* listener)
    {
        modelListener = listener;
    }

    void tick();
    void updateHumidifierControlState(bool state);
protected:
    ModelListener* modelListener;
    int sensorUpdatePeriod;
};

#endif // MODEL_HPP
