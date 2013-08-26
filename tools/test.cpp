
int main()
{
    int a, b, c, d;
    char buffer[20];

    a = 99343; b = 1135;
    c = a + b;
    d = a * b;

    char p = 'C';

    for(int i = 0; i < 20; i++)
    {
        buffer[i] = p + i;

        if(buffer[i] == 'P')
           buffer[i] = 'Z';
    }

    return 0;
}
