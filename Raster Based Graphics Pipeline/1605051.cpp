#include<iostream>
#include<algorithm>
#include<vector>
#include<stack>
#include<iomanip>
#include<math.h>
#include<cstdio>
#include<stdlib.h>
#include<string.h>
#include<time.h>

#include "bitmap_image.hpp"

using namespace std;

FILE *stage1, *stage2, *stage3, *scene, *config, *z_buffer;
double fov_y, fov_x, aspect_ratio, near, far;
void task4();
double deg_to_rad(double degree)
{
    return acos(-1.0) * degree / 180;
}


class Point {
public:
    double x, y, z, w;
    Point()
    {
        x = y = z = 0;
        w = 1;
    }
    Point(double x, double y, double z)
    {
        this->x = x;
        this->y = y;
        this->z = z;
        this->w = 1;
    }
    void Print()
    {
        cout << fixed << setprecision(1);
        cout << "(" << x << " " << y << " " << z << ")" << endl;
    }
    void Print(FILE *fp)
    {
        fprintf(fp, "%0.7lf %0.7lf %0.7lf\n", x, y, z);
    }
    void normalize()
    {
        double val = sqrt(x * x + y * y + z * z);
        x /= val, y /= val, z /= val;
    }
} eye, look, up;

Point sub(Point a, Point b)
{
    Point res;
    res.x = a.x - b.x;
    res.y = a.y - b.y;
    res.z = a.z - b.z;
    return res;
}

Point add(Point a, Point b)
{
    Point res;
    res.x = a.x + b.x;
    res.y = a.y + b.y;
    res.z = a.z + b.z;
    return res;
}

Point scalar_mult(double value, Point p)
{
    Point res;
    res.x = p.x * value;
    res.y = p.y * value;
    res.z = p.z * value;
    return res;
}

Point cross_product(Point a, Point b)
{
    Point res;
    res.x = a.y * b.z - a.z * b.y;
    res.y = a.z * b.x - a.x * b.z;
    res.z = a.x * b.y - a.y * b.x;
    return res;

}
double dot_product(Point a, Point b)
{
    return (a.x * b.x + a.y * b.y + a.z * b.z);
}

Point Rodrigues(Point axis, Point rotation_vector, double angle)
{
    Point res = scalar_mult(cos(angle), axis);
    double res1 = dot_product(rotation_vector, axis);
    res1 *= (1 - cos(angle));
    res = add(scalar_mult(res1, rotation_vector), res);
    res = add(res, scalar_mult(sin(angle), cross_product(rotation_vector, axis)));
    return res;
}

class Matrix
{
public:
    double matrix[4][4];
    Matrix()
    {
        identity();
    }

    void identity()
    {
        memset(matrix, 0, sizeof matrix);
        for (int i = 0; i < 4; i++) matrix[i][i] = 1;
    }

    void Print()
    {
        cout << fixed << setprecision(2);
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                cout << matrix[i][j] << " ";
            }
            cout << endl;
        }
    }

    Point multiply(Point point)
    {
        Point res;
        res.x = matrix[0][0] * point.x + matrix[0][1] * point.y + matrix[0][2] * point.z + matrix[0][3] * point.w;
        res.y = matrix[1][0] * point.x + matrix[1][1] * point.y + matrix[1][2] * point.z + matrix[1][3] * point.w;
        res.z = matrix[2][0] * point.x + matrix[2][1] * point.y + matrix[2][2] * point.z + matrix[2][3] * point.w;
        res.w = matrix[3][0] * point.x + matrix[3][1] * point.y + matrix[3][2] * point.z + matrix[3][3] * point.w;
        res.x /= res.w, res.y /= res.w, res.z /= res.w, res.w = 1;
        return res;
    }

     Matrix multiply(Matrix mat)
    {
        Matrix temp;

        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 4; j++)
            {
                double sum = 0;
                for (int k = 0; k < 4; k++)
                {
                    sum += matrix[i][k] * mat.matrix[k][j];
                }
                temp.matrix[i][j] = sum;
            }
        }
        return temp;
    }

};

Matrix temp_matrix, current_matrix;
stack<pair<Matrix, bool> > matrix_stack;

int main()
{
    scene = fopen("scene.txt", "r");
    stage1 = fopen("stage1.txt", "w");

    fscanf(scene, "%lf %lf %lf %lf %lf %lf %lf %lf %lf",
           &eye.x, &eye.y, &eye.z, &look.x, &look.y, &look.z, &up.x, &up.y, &up.z);
    fscanf(scene, "%lf %lf %lf %lf", &fov_y, &aspect_ratio, &near, &far);

    matrix_stack.push({Matrix(), false});

    while (true)
    {
        char input[50];
        fscanf(scene, "%s", input);
        //printf(input);
        //printf("\n");


        if (strcmp(input, "triangle") == 0)
        {
            Point temp_point;
            for (int i = 0; i < 3; i++)
            {
                fscanf(scene, "%lf %lf %lf", &temp_point.x, &temp_point.y, &temp_point.z);
                temp_point.w = 1;

                temp_point = matrix_stack.top().first.multiply(temp_point);
                temp_point.Print(stage1);
            }
            fprintf(stage1, "\n");

        }
        else if (strcmp(input, "translate") == 0)
        {
            temp_matrix.identity();
            fscanf(scene, "%lf %lf %lf", &temp_matrix.matrix[0][3], &temp_matrix.matrix[1][3], &temp_matrix.matrix[2][3]);
            temp_matrix = matrix_stack.top().first.multiply(temp_matrix);
            matrix_stack.push({temp_matrix, false});
        }
        else if (strcmp(input, "scale") == 0)
        {
            temp_matrix.identity();
            fscanf(scene, "%lf %lf %lf", &temp_matrix.matrix[0][0], &temp_matrix.matrix[1][1], &temp_matrix.matrix[2][2]);
            temp_matrix = matrix_stack.top().first.multiply(temp_matrix);
            matrix_stack.push({temp_matrix, false});

        }
        else if (strcmp(input, "rotate") == 0)
        {
            double angle, ax, ay, az;
            fscanf(scene, "%lf %lf %lf %lf", &angle, &ax, &ay, &az);

            Point a = Point(ax, ay, az);
            a.normalize();
            Point c1 = Rodrigues(Point(1, 0, 0), a, deg_to_rad(angle));
            Point c2 = Rodrigues(Point(0, 1, 0), a, deg_to_rad(angle));
            Point c3 = Rodrigues(Point(0, 0, 1), a, deg_to_rad(angle));
            temp_matrix.identity();
            temp_matrix.matrix[0][0] = c1.x;
            temp_matrix.matrix[1][0] = c1.y;
            temp_matrix.matrix[2][0] = c1.z;
            temp_matrix.matrix[0][1] = c2.x;
            temp_matrix.matrix[1][1] = c2.y;
            temp_matrix.matrix[2][1] = c2.z;
            temp_matrix.matrix[0][2] = c3.x;
            temp_matrix.matrix[1][2] = c3.y;
            temp_matrix.matrix[2][2] = c3.z;

            temp_matrix = matrix_stack.top().first.multiply(temp_matrix);
            matrix_stack.push({temp_matrix, false});

        }
        else if (strcmp(input, "push") == 0)
        {
            matrix_stack.push({matrix_stack.top().first, true});

        }
        else if (strcmp(input, "pop") == 0)
        {
            while (!matrix_stack.empty() && !matrix_stack.top().second) matrix_stack.pop();
            if (!matrix_stack.empty()) matrix_stack.pop();
        }
        else if (strcmp(input, "end") == 0) break;
    }

    //matrix_stack.top().first.Print();

    fclose(scene);
    fclose(stage1);

    stage1 = fopen("stage1.txt", "r");
    stage2 = fopen("stage2.txt", "w");
    stage3 = fopen("stage3.txt", "w");

    Point l = sub(look, eye);
    l.normalize();
    Point r = cross_product(l, up);
    r.normalize();
    Point u = cross_product(r, l);

    temp_matrix.identity();
    temp_matrix.matrix[0][3] = -eye.x;
    temp_matrix.matrix[1][3] = -eye.y;
    temp_matrix.matrix[2][3] = -eye.z;

    Matrix R;
    R.matrix[0][0] = r.x, R.matrix[0][1] = r.y, R.matrix[0][2] = r.z;
    R.matrix[1][0] = u.x, R.matrix[1][1] = u.y, R.matrix[1][2] = u.z;
    R.matrix[2][0] = -l.x, R.matrix[2][1] = -l.y, R.matrix[2][2] = -l.z;

    Matrix V = R.multiply(temp_matrix);

    fov_x = fov_y * aspect_ratio;
    double t = near * tan(deg_to_rad(fov_y) / 2);
    double rr = near * tan(deg_to_rad(fov_x) / 2);

    Matrix P;
    P.matrix[0][0] = near / rr;
    P.matrix[1][1] = near / t;
    P.matrix[2][2] = -(far + near) / (far - near);
    P.matrix[3][3] = 0;
    P.matrix[2][3] = -( 2 * far * near) / (far - near);
    P.matrix[3][2] = -1;

    int cnt = 0;
    Point temp_point;

    while (fscanf(stage1, "%lf %lf %lf", &temp_point.x, &temp_point.y, &temp_point.z) != EOF)
    {
        temp_point.w = 1;

        cnt++;
        temp_point = V.multiply(temp_point);
        temp_point.Print(stage2);

        temp_point = P.multiply(temp_point);
        temp_point.Print(stage3);

        if (cnt % 3 == 0) fprintf(stage2, "\n"), fprintf(stage3, "\n");
    }

    fclose(stage2);
    fclose(stage1);
    fclose(stage3);

    task4();


    return 0;
}


class Color
{
public:
    int r, g, b;
    Color()
    {
        r = 0, g = 0, b = 0;
    }
    Color (int r, int g, int b)
    {
        this->r = r;
        this->b = b;
        this->g = g;
    }
};

vector<vector<double> > z_buffer_array;
vector<vector<Color> > frame_buffer;

void task4()
{
    srand(time(NULL));
    config = fopen("config.txt", "r");
    z_buffer = fopen("z_buffer.txt", "w");
    stage3 = fopen("stage3.txt", "r");

    int screen_width, screen_height;
    double left_limit, bottom_limit;
    double front_limit, rear_limit;

    fscanf(config, "%d %d %lf %lf %lf %lf",
           &screen_width, &screen_height, &left_limit, &bottom_limit,
           &front_limit, &rear_limit);

    //cout << screen_width << " " << screen_height << endl;
    //cout << left_limit << " " << bottom_limit << endl;
    //cout << front_limit << " " << rear_limit << endl;
    z_buffer_array.assign(screen_height + 5, vector<double>(screen_width + 5, rear_limit));
    frame_buffer.assign(screen_height + 5, vector<Color> (screen_width + 5, Color() ));

    double dx = (-left_limit - left_limit) / screen_width;
    double dy = (-bottom_limit - bottom_limit) / screen_height;

    double top_y = -bottom_limit - dy / 2;
    double left_x = left_limit + dx / 2;

    Point x, y, z;
    while (fscanf(stage3, "%lf %lf %lf", &x.x, &x.y, &x.z) != EOF)
    {
        fscanf(stage3, "%lf %lf %lf", &y.x, &y.y, &y.z);
        fscanf(stage3, "%lf %lf %lf", &z.x, &z.y, &z.z);

        Color cur_color = Color(abs(rand()) % 255, abs(rand()) % 255, abs(rand()) % 255);

        int top_offset = 0;
        while (top_y -top_offset * dy > max(bottom_limit + dy / 2, max(x.y, max(y.y, z.y)))) top_offset++;

        while (top_y - top_offset * dy >= max(bottom_limit + dy / 2, min(x.y, min(y.y, z.y))))
        {
            double cur_y = top_y - top_offset * dy;

            double za, zb, xa, xb;
            bool flag = false;
            if (cur_y < max(y.y, x.y) && cur_y > min(y.y, x.y))
            {
                za = x.z + (cur_y - x.y) * (y.z - x.z) / (y.y - x.y);
                xa = x.x + (cur_y - x.y) * (y.x - x.x) / (y.y - x.y);

                if (cur_y < max(z.y, x.y) && cur_y > min(z.y, x.y))
                {
                    zb = x.z + (cur_y - x.y) * (z.z - x.z) / (z.y - x.y);
                    xb = x.x + (cur_y - x.y) * (z.x - x.x) / (z.y - x.y);
                }
                else if (cur_y < max(z.y, y.y) && cur_y > min(z.y, y.y))
                {
                    zb = y.z + (cur_y - y.y) * (z.z - y.z) / (z.y - y.y);
                    xb = y.x + (cur_y - y.y) * (z.x - y.x) / (z.y - y.y);
                }
                else flag = true;
            }
            else if (cur_y < max(z.y, x.y) && cur_y > min(z.y, x.y))
            {
                zb = x.z + (cur_y - x.y) * (z.z - x.z) / (z.y - x.y);
                xb = x.x + (cur_y - x.y) * (z.x - x.x) / (z.y - x.y);

                if (cur_y < max(z.y, y.y) && cur_y > min(z.y, y.y))
                {
                    za = y.z + (cur_y - y.y) * (z.z - y.z) / (z.y - y.y);
                    xa = y.x + (cur_y - y.y) * (z.x - y.x) / (z.y - y.y);
                }
                else flag = true;
            }
            else flag = true;

            if (flag) continue;

            ///printf("%lf %lf %lf %lf\n", za, zb, xa, xb);

            int left_offset = 0;

            if (xa > xb)
            {
                swap(xa, xb);
                swap(za, zb);
            }

            while (left_x + dx * left_offset < min(xa, -left_x))
            {
                left_offset++;
            }

            while (left_x + dx * left_offset <= min(xb, -left_x) && fabs(xb - xa) > 1e-9)
            {
                double xp = left_x + dx * left_offset;
                double zp = za + (xp - xa) * (zb - za) / (xb - xa);
                //printf("%lf\n", zp);
                if (zp > front_limit && zp < z_buffer_array[top_offset][left_offset])
                {
                    z_buffer_array[top_offset][left_offset] = zp;
                    frame_buffer[top_offset][left_offset] = cur_color;
                    //fprintf(z_buffer, "%0.6lf\t", zp);
                }
                left_offset++;
            }
            //fprintf(z_buffer, "\n");

            top_offset++;

        }

    }

    for (int i = 0; i < screen_height; i++)
    {
        for (int j = 0; j < screen_width; j++)
        {
            if (z_buffer_array[i][j] < rear_limit - 0.000005)
                fprintf(z_buffer, "%0.6lf\t", z_buffer_array[i][j]);
        }
        fprintf(z_buffer, "\n");
    }

    fclose(z_buffer);
    fclose(stage3);
    fclose(config);


    bitmap_image image(screen_width, screen_height);

    for (int i = 0; i < screen_height; i++)
    {
        for (int j = 0; j < screen_width; j++)
        {
            image.set_pixel(j, i, frame_buffer[i][j].r, frame_buffer[i][j].g, frame_buffer[i][j].g);

        }
    }

    image.save_image("output.bmp");
}
