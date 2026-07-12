
#include "header.h"
using namespace std;

//this helper used in the taylor series
double pwrFunc(double base, int power)
{
    if (power == 0)
    {
        return 1;
    }
    double multiplier = 1;
    int idx = 1;
    while (idx <= power)
    {
        multiplier *= (double)base;
        idx++;
    }

    return multiplier;
}
//helper for factorial
double factFunc(int n)
{
    if (n == 0)
    {
        return 1.0;
    }
    double multiplier = 1;
    int i = 1;
    while (i <= n)
    {
        multiplier *= i;
        i++;
    }
    return multiplier;
}
double sintheta(double theta)
{
    double pi = 3.14159265358979;
    //clamping
    while (theta > pi)
    {
        theta = theta - (2 * pi); //to bring it in the range of -pi to pi
    }
    while (theta < -pi)
    {
        theta = theta + (2 * pi);
    }
    //calculating the sin theta value using the helpers created
    double sum = 0;
    int i = 1;

    while (i <= 6)
    {
        int power = 2 * i - 1;
        if (i % 2 == 1)
        {
            //using the taylor series sin formula for calculating the sin theta value. for odd + and even -   
            sum = sum + (pwrFunc(theta, power) / factFunc(power));
        }
        else
        {
            //since i is even, multiply the value on the ith position with -    
            sum = sum - (pwrFunc(theta, power) / factFunc(power));
        }
        i++;
    }
    return sum;
}
double costheta(double theta)
{
    double pi = 3.14159265358979;
    //performing clamping to bring within the range
    while (theta > pi)
    {
        theta = theta - (2 * pi); //to bring it in the range of -pi to pi
    }
    while (theta < -pi)
    {
        theta = theta + (2 * pi);
    }

    double sum = 0;
    int i = 0;
    while (i < 6)
    {   //using the taylor series formula for calculating cos theta value
        int pwr = 2 * i;
        if (i % 2 == 0)
        {
            //since even position, then +
            sum = sum + (pwrFunc(theta, pwr) / factFunc(pwr));
        }
        else
        {
            //since odd position, -   
            sum = sum - (pwrFunc(theta, pwr) / factFunc(pwr));
        }
        i++;
    }
    return sum;
}
Complex::Complex()
{
    this->r = 0.0f;
    this->i = 0.0f;
}
Complex::Complex(float r, float i)
{
    this->i = i;
    this->r = r;
}
Complex Complex::operator+(const Complex& cpy)
{
    return Complex(this->r + cpy.r, this->i + cpy.i);
}
Complex Complex::operator-(const Complex& cpy)
{
    return Complex(this->r - cpy.r, this->i - cpy.i);
}
Complex Complex::operator*(const Complex& cpy)
{
    return Complex(((this->r * cpy.r) - (this->i * cpy.i)), ((this->r * cpy.i) + (this->i * cpy.r)));
}

Filter::Filter()
{
    w = nullptr;
    sizeOfW = 0;
    smpleRate = 0; //sample rate 
};
Filter::Filter(int size_w, int smplRate)
{
    sizeOfW = size_w;
    this->smpleRate = smplRate;
    w = new Complex[sizeOfW];
    int i = 0;
    while (i < sizeOfW)
    {
        (*(w + i)).r = float(1.0f); //initialising all positions elements to 1.0f
        (*(w + i)).i = 0.0f;
        i++;
    }
}
Filter :: ~Filter()
{
    if (w != nullptr)
    {
        delete[] w; //ensuring that no memory leak
    }
}
Filter::Filter(const Filter& cpy)
{
    sizeOfW = cpy.sizeOfW;
    smpleRate = cpy.smpleRate;
    w = new Complex[sizeOfW];
    int i = 0;
    while (i < sizeOfW)
    {
        *(w + i) = *(cpy.w + i);
        i++;
    }
}
Filter& Filter:: operator= (const Filter& cpy)
{
    if (this != &cpy) //to avoid assigning to itself
    {
        delete[] w; //deleting the old ptr first

        sizeOfW = cpy.sizeOfW;
        smpleRate = cpy.smpleRate;
        w = new Complex[sizeOfW];
        int i = 0;
        while (i < sizeOfW)
        {
            *(w + i) = *(cpy.w + i);
            i++;
        }
    }
    return *this;

}
void Filter::lpFilter(float highlim)
{
    //finding the index first
    int idx = 0;
    int product = (highlim * sizeOfW);
    if (smpleRate != 0)
    {
        idx = product / smpleRate;
        int i = idx;
        //applying only till half of the size; from idx till half
        //idx is the upper limit and upper limit is ALWAYS less than mid so upper limit till mid
        //sizeOfW/2 is highest frequency
        while (i <= sizeOfW / 2)
        {	//as i gets closer to the max/mid
            (*(w + i)).r = 0.0f;
            (*(w + i)).i = 0.0f;
            //for the mirror effect, ensuring that the end is also being erased
            if (i >= 1)
            {
                (*(w + sizeOfW - i)).r = 0.0f;
                (*(w + sizeOfW - i)).i = 0.0f;
            }
            i++;
        }
    }
}
void Filter::hpFilter(float lowlim)
{	//in this, starting at the beginning till the limit, clearing all elements
    int idx = 0; //the index stroed in it by using frequency and sample size
    int product = (lowlim * sizeOfW);
    if (smpleRate != 0)
    {
        idx = product / smpleRate;
        int i = 1;
        while (i < idx && i <= sizeOfW / 2)
        {
            (*(w + i)).r = 0.0f;
            (*(w + i)).i = 0.0f;
            //for the mirror as well (starting from end indexes)
            if (i >= 1)
            {
                (*(w + sizeOfW - i)).r = 0.0f;
                (*(w + sizeOfW - i)).i = 0.0f;

            }
            i++;
        }
    }
}
void Filter::bpFilter(float lowlim, float highlim)
{
    int ilow = 0;
    int ihigh = 0;
    int pLow = (lowlim * sizeOfW);
    int pHigh = (highlim * sizeOfW);
    //ensuring that the divisor isn't a 0 and size exists
    if (smpleRate != 0 && sizeOfW != 0)
    {
        ilow = pLow / smpleRate;
        ihigh = pHigh / smpleRate;
        int i = 0;
        //loop till half the size only
        while (i <= sizeOfW / 2)
        {	//for indexes not in the range 
            if (i < ilow || i > ihigh)
            {
                (*(w + i)).r = 0.0f;
                (*(w + i)).i = 0.0f;
                if (i >= 1 && i <= (sizeOfW / 2) - 1)
                {
                    (*(w + sizeOfW - i)).r = 0.0f;
                    (*(w + sizeOfW - i)).i = 0.0f;
                }
            }
            i++;
        }
    }
}

Spectrum::Spectrum()
{
    fBin = nullptr; oSize = 0; smplRate = 0; pSize = 0;
}
Spectrum::Spectrum(int oSize, int pSize, int smplRate)
{
    this->oSize = oSize;
    this->pSize = pSize;
    this->smplRate = smplRate;
    fBin = new Complex[pSize];
    int i = 0;
    while (i < pSize)
    {
        *(fBin + i) = Complex(0.0f, 0.0f); //initialising all positions elements to 0
        i++;
    }
}
Spectrum::~Spectrum()
{
    if (fBin != nullptr)
    {
        delete[] fBin; //ensuring that no memory leak
    }
}
Spectrum::Spectrum(const Spectrum& cpy)
{
    pSize = cpy.pSize;
    oSize = cpy.oSize;
    smplRate = cpy.smplRate;
    fBin = new Complex[pSize];
    int i = 0;
    while (i < pSize)
    {
        *(fBin + i) = *(cpy.fBin + i);
        i++;
    }
}
Spectrum& Spectrum:: operator= (const Spectrum& cpy)
{
    if (this != &cpy) //to avoid assigning to itself
    {
        delete[] fBin; //deleting the old ptr first

        pSize = cpy.pSize;
        oSize = cpy.oSize;
        smplRate = cpy.smplRate;
        fBin = new Complex[pSize];
        int i = 0;
        while (i < pSize)
        {
            *(fBin + i) = *(cpy.fBin + i);
            i++;
        }
    }
    return *this;

}
void Spectrum::filteration(Filter& f)
{
    int i = 0;
    while (i < pSize)
    {
        float w = (*(f.w + i)).r; //f is filter array that stores weights so extract weight on index i
        (*(fBin + i)).r = (*(fBin + i)).r * w;
        (*(fBin + i)).i = (*(fBin + i)).i * w;

        i++;
    }
}

Whisper Spectrum::i_FFT()
{
    int n = (int)log2(pSize);
    int i = 0;
    while (i < pSize)
    {
        int r = 0, count = 0;
        while (count < n)
        {
            //extract bit at count position
            int current = (i >> count) & 1; //count times right shift to extract bit over there
            r = (r << 1) | (current << 0); //r is left shifted then current bit placed at the newly created index. basically space is created by doing left shift r and current is dropped in
            count++;
        }
        if (i < r)
        {
            Complex tmp = *(fBin + i);
            *(fBin + i) = *(fBin + r);
            *(fBin + r) = tmp;
        }
        i++;
    }
    int distanceBetween2Elements = 1; //the distance between the 2 elements that are being compared
    while (distanceBetween2Elements < pSize) //later the distance between the 2 elements changes to the jump size. so as long as the jump size doesn't exceed the padded size
    {
        int skippingElementsUnits = distanceBetween2Elements * 2; //it becomes twice every round
        double angleDivision = 2.0 * 3.14159265358979 / (double)skippingElementsUnits;//the skippingElementsUnits decides how the division of the complete circle will be done e.g when skippingElementsUnits = 2, divide the circle into 2 to get 180 degrees
        int start = 0;
        while (start < pSize)
        {
            int currentIdx = 0;
            while (currentIdx < distanceBetween2Elements)
            {
                double angleForCurrentIdx = (double)currentIdx * angleDivision; //the elements involved within the range distanceBetween2Elements, the boundaries inclusive, have their own turning angle 
                float cosAngle = (float)costheta(angleForCurrentIdx);
                float sinAngle = (float)sintheta(angleForCurrentIdx);
                Complex weight(cosAngle, sinAngle);
                //pointers to the 2 elements being compared
                Complex* element1 = fBin + start + currentIdx;
                Complex* element2 = element1 + distanceBetween2Elements; //second element is going to be that many jump away
                Complex rotationOnElement2 = (*element2) * weight;
                Complex tempelement1 = *element1;
                //performing constructive and destructive interference
                //constructive to increase amplitude and destructive to minimise
                //plus to get common sound and minus to get the difference
                *element1 = tempelement1 + rotationOnElement2;
                *element2 = tempelement1 - rotationOnElement2;

                currentIdx++;
            }
            //the starting now shifts skippingElementsUnits times (adding as an offset)
            start = start + skippingElementsUnits;
        }
        //and distanceBetween2Elements updates to skippingElementsUnits
        distanceBetween2Elements = skippingElementsUnits;
    }
    //creating an object that stores the final audio features
    Whisper storingFinalAud;
    storingFinalAud.numOfSamples = oSize;
    storingFinalAud.hdr.sampleRate = smplRate;
    storingFinalAud.audioStr = new short[oSize]; //this size without the padding or the zeroes/only holds the real data
    int m = 0;
    if (pSize > 0) //ensuring that not divided by 0
    {
        while (m < oSize)
        {	//division happens to get back the original volume
            float normaliseVol = (*(fBin + m)).r / (float)pSize;
            *(storingFinalAud.audioStr + m) = (short)normaliseVol; //short because 16 bit integers required
            m++;
        }
    }
    else
    {
        while (m < oSize)
        {
            *(storingFinalAud.audioStr + m) = 0;
            m++;
        }
    }
    return storingFinalAud;
}

Whisper::Whisper() : hdr{}, audioStr(nullptr), numOfSamples(0) {}
Whisper::~Whisper()
{
    if (audioStr != nullptr)
    {
        delete[] audioStr;
        audioStr = nullptr;
    }
}
Whisper::Whisper(const char* file) : hdr{}, audioStr(nullptr), numOfSamples(0)
{
    ifstream encFile(file, ios::binary);
    if (!encFile)
    {
        cout << "Couldnot open! Exiting :(" << endl;
        return;
    }
    encFile.read((char*)&hdr, sizeof(WavHeader));
    this->numOfSamples = ((hdr.subchunk2Size) * 8) / (hdr.bitsPerSample);
    this->audioStr = new short[numOfSamples];
    if (hdr.bitsPerSample == 16)
    {
        encFile.read((char*)audioStr, hdr.subchunk2Size);
    }
    else if (hdr.bitsPerSample == 8)
    {
        unsigned char* tmp = new unsigned char[numOfSamples];
        encFile.read((char*)tmp, hdr.subchunk2Size);
        for (int i = 0; i < numOfSamples; i++)
        {   //conversion of unsigned char into a short
            //temp-> array of unsigned char; 0-255 where 128 is muted point
            //*(tmp + i) gives a number that is shifted down to 0 through -128
            //multiplying the result with 256 to amplify since 65536 / 256 = 256
            *(audioStr + i) = (*(tmp + i) - 128) * 256;
        }
        delete[] tmp;
        //updating the wavheader member variables
        hdr.byteRate = (hdr.sampleRate * hdr.numChannels) * 2;
        hdr.subchunk2Size = 2 * numOfSamples;
        hdr.blockAlign = 2 * hdr.numChannels;
        hdr.bitsPerSample = 16;

    }
    encFile.close();
}
//copy constructor
Whisper::Whisper(const Whisper& cpy) : hdr(cpy.hdr), numOfSamples(cpy.numOfSamples)
{
    if (cpy.audioStr != nullptr)
    {
        this->audioStr = new short[numOfSamples];
        int i = 0;
        while (i < numOfSamples)
        {
            *(this->audioStr + i) = *(cpy.audioStr + i);
            i++;
        }
    }
    else
    {
        this->audioStr = nullptr;
    }
}
Whisper& Whisper::operator=(const Whisper& cpy)
{   //to avoid assigning to itself
    if (this != &cpy)
    {
        delete[] audioStr; //deleting the old ptr first
        numOfSamples = cpy.numOfSamples;
        hdr = cpy.hdr;
        audioStr = new short[numOfSamples];
        int i = 0;
        while (i < numOfSamples)
        {
            *(audioStr + i) = *(cpy.audioStr + i);
            i++;
        }
    }
    return *this;
}
void Whisper::audReversal()
{
    short* tmp = new short[numOfSamples];
    int i = 0;
    while (i<numOfSamples)
    {
        *(tmp + i) = *(audioStr + numOfSamples - i - 1);
        i++;
    }
    int k = 0;
    while (k < numOfSamples)
    {
        *(audioStr + k) = *(tmp + k);
        k++;
    }
    delete[] tmp;
}
Whisper& Whisper :: operator*=(float gain)
{
    for (int i = 0; i < numOfSamples; i++)
    {
        float val = *(audioStr + i) * gain;
        *(audioStr + i) = (val > 32767.0f) ? 32767.0f : ((val < -32767.0f) ? -32767.0f : (short)val);
    }
    return *this;
}
Whisper Whisper :: operator+(const Whisper& other)
{
    int size = (numOfSamples > other.numOfSamples) ? numOfSamples : other.numOfSamples;
    Whisper sum;
    sum.numOfSamples = size;
    sum.audioStr = new short[size];
    if (numOfSamples > other.numOfSamples)
    {
        sum.hdr = this->hdr;
    }
    else
    {
        sum.hdr = other.hdr;
    }
    for (int i = 0; size; i++)
    {
        if (i < other.numOfSamples && i < numOfSamples)
        {
            float val = *(audioStr + i) + *(other.audioStr + i);
            *(sum.audioStr + i) = (val > 32767.0f) ? 32767.0f : ((val < -32767.0f) ? -32767.0f : val);
        }
        else if (i >= other.numOfSamples && i < numOfSamples)
        {
            float val = *(audioStr + i);
            *(sum.audioStr + i) = (val > 32767.0f) ? 32767.0f : ((val < -32767.0f) ? -32767.0f : val);
        }
        else if (i < other.numOfSamples && i >= numOfSamples)
        {
            float val = *(other.audioStr + i);
            *(sum.audioStr + i) = (val > 32767.0f) ? 32767.0f : ((val < -32767.0f) ? -32767.0f : val);
        }
    }
    return sum;
}
Whisper& Whisper:: operator+=(const Whisper& other)
{
    int sizeAdded = numOfSamples + other.numOfSamples;
    short* tempArr = new short[sizeAdded];
    for (int i = 0; i < sizeAdded; i++)
    {
        if (i < numOfSamples)
        {
            *(tempArr + i) = *(audioStr + i);
        }
        else
        {
            *(tempArr + i) = *(other.audioStr + i - numOfSamples);
        }
    }
    delete[] audioStr;
    numOfSamples = sizeAdded;
    audioStr = tempArr;
    hdr.subchunk2Size = numOfSamples * 2; //since short has two bytes

    return *this;
}
void Whisper::pNormal()
{
    short max = 0;
    for (int i = 0; i < numOfSamples; i++)
    {
        float val = *(audioStr + i);
        if (*(audioStr + i) < 0)
        {
            val = -1 * (*(audioStr + i));
        }
        if (val > max)
        {
            max = val;
        }
    }
    if (max > 0)
    {
        float r = 32767 / max; //scaling horahi hay
        for (int i = 0; i < numOfSamples; i++)
        {
            *(audioStr + i) *= r;
        }
    }
}
Whisper Whisper::splice(float s, float e)
{

    int idxStart = s * (hdr.sampleRate);
    int idxEnd = e * (hdr.sampleRate);
    bool startFound = false;
    idxStart = (idxStart < 0)? 0 : idxStart;
    idxEnd = (idxEnd > numOfSamples)? numOfSamples : idxEnd;
    if (idxStart >= idxEnd)
    {
        cout << "OOPS! Invalid Range Entered :(\n";
        return *this;
    }
    Whisper chunk;
    int newSize = idxEnd - idxStart;
    chunk.numOfSamples = newSize;
    chunk.audioStr = new short[newSize];
    int j = 0;
    for (int i = idxStart; i < idxEnd; i++)
    {
        //copying the data from original stream to chunk's audio stream
        *(chunk.audioStr + j) = *(audioStr + i);
        j++;
    }
    //updating hdr
    chunk.hdr = this->hdr;
    chunk.hdr.subchunk2Size = newSize * 2;
    return chunk;

}

void Whisper::downld(const char* outName) {
    ofstream of(outName, std::ios::binary);
    of.write((char*)&hdr, sizeof(WavHeader));
    of.write((char*)audioStr, hdr.subchunk2Size);
    of.close();
}

void Whisper::loadinc(Spectrum& s)
{
    int i = 0;
    while (i < numOfSamples)
    {
        (*(s.fBin + i)).r = (float)(*(audioStr + i));
        (*(s.fBin + i)).i = 0.0f;
        i++;
    }
    int j = i;
    while (j<s.pSize)
    {
        (*(s.fBin + j)).r = 0.0f;
        (*(s.fBin + j)).i = 0.0f;
        j++;
    }
}
Spectrum Whisper::fwdTransform()
{
    //calculating the next power of two
    int pSize = 1;
    while (pSize < numOfSamples)
    {
        pSize = pSize * 2;
    }
    //now i have the padded size which is the total number of audio samples as perfect power of 2
    //loading the spectrum here by calling the function
    Spectrum s(numOfSamples, pSize, hdr.sampleRate);
    loadinc(s);
    //finding the power of two that equals the pSize
    int n = (int)log2(pSize);
    int i = 0;
    while (i < pSize)
    {
        int r = 0, count = 0;
        while (count < n)
        {
            //extract bit at count position
            int current = (i >> count) & 1; //count times right shift to extract bit over there
            r = (r << 1) | (current << 0); //r is left shifted then current bit placed at the newly created index. basically space is created by doing left shift r and current is dropped in
            count++;
        }
        if (i < r)
        {
            Complex tmp = *(s.fBin + i);
            *(s.fBin + i) = *(s.fBin + r);
            *(s.fBin + r) = tmp;
        }
        i++;
    }
    int distanceBetween2Elements = 1; //the distance between the 2 elements that are being compared
    while (distanceBetween2Elements < pSize) //later the distance between the 2 elements changes to the jump size. so as long as the jump size doesn't exceed the padded size
    {
        int skippingElementsUnits = distanceBetween2Elements * 2; //it becomes twice every round
        double angleDivision = -2.0 * 3.14159265358979 / (double)skippingElementsUnits;//the skippingElementsUnits decides how the division of the complete circle will be done e.g when skippingElementsUnits = 2, divide the circle into 2 to get 180 degrees
        int start = 0;
        while (start < pSize)
        {
            int currentIdx = 0;
            while (currentIdx < distanceBetween2Elements)
            {
                double angleForCurrentIdx = (double)currentIdx * angleDivision; //the elements involved within the range distanceBetween2Elements, the boundaries inclusive, have their own turning angle 
                float cosAngle = (float)costheta(angleForCurrentIdx);
                float sinAngle = (float)sintheta(angleForCurrentIdx);
                Complex weight(cosAngle, sinAngle);
                //pointers to the 2 elements being compared
                Complex* element1 = s.fBin + start + currentIdx;
                Complex* element2 = element1 + distanceBetween2Elements; //second element is going to be that many jump away
                Complex rotationOnElement2 = (*element2) * weight;
                Complex tempelement1 = *element1;
                //performing constructive and destructive interference
                //constructive to increase amplitude and destructive to minimise
                //plus to get common sound and minus to get the difference
                *element1 = tempelement1 + rotationOnElement2;
                *element2 = tempelement1 - rotationOnElement2;

                currentIdx++;
            }
            //the starting now shifts skippingElementsUnits times (adding as an offset)
            start = start + skippingElementsUnits;
        }
        //and distanceBetween2Elements updates to skippingElementsUnits
        distanceBetween2Elements = skippingElementsUnits;
    }
    return s;
}
int main()
{
    cout << "--- Welcome to the Audio Processing Engine ---" << endl;
    Whisper wArr [4];
    Spectrum sArr [4];
    int activewhisp = 0, activespect = 0;
    bool isRunning = true;
    while (isRunning)
    {
        int choice;
        cout<<"\nSelect from the three:\n1. Time Domain Menue\n2. Frequency Domain Menu\n3. Exit\n";
        cin>>choice;
        if (cin.fail() || choice<1 ||choice>3)
        {   
            cin.clear();
            cin.ignore(10000, '\n');
            cout <<"Invalid Input! Re-enter the option : ";
            cin>>choice;
        }
        if (choice == 1)
        {
            int choiceNum;
            cout <<"\n--- Welcome to the Time Domain Menu ---\n";
            cout << "1. Loading Audio File\n2. Audio Reversal\n3. Audio Normalisation\n4. Audio Splicing\n\n";
            cout << "Choice: ";
            cin>> choiceNum;
            while (cin.fail() || choiceNum < 1 || choiceNum > 4)
            {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "\nInvalid Input for file! Re-enter option 1, 2, 3, or 4: ";
                cin >> choiceNum;
            }
            if (choiceNum == 1)
            {
                int fileNum;
                cout << "\nSelect audio file (1, 2, 3, or 4) to load in slot " << (activewhisp + 1) << " : ";
                cout << "\n1. sample.wav\n2. sample2.wav (Khursheed Abdullah)\n3. sample3.wav\n4. sample4.wav\nChoice: ";
                cin >> fileNum;
                while (cin.fail() || fileNum < 1 || fileNum > 4)
                {
                    cin.clear();
                    cin.ignore(10000, '\n');
                    cout << "\nInvalid Input! Re-enter option 1, 2, 3, or 4: ";
                    cin >> fileNum;
                }
                if (fileNum == 1)
                {
                    *(wArr + activewhisp) = Whisper("sample.wav");
                }
                else if (fileNum == 2)
                {
                    *(wArr + activewhisp) = Whisper("sample2.wav");
                }
                else if (fileNum == 3)
                {
                    *(wArr + activewhisp) = Whisper("sample 3.wav");
                }
                else if (fileNum == 4)
                {
                    *(wArr + activewhisp) = Whisper("sample 4.wav");
                }
                cout << "\n-- Audio has been loaded successfully --\n" << endl;
            }
            else
            {
                if ((*(wArr + activewhisp)).numOfSamples == 0)
                {
                    cout << "Oops! Seems like you have not selected [1]. Select that first to proceed..." << endl;
                    continue;
                }
                if (choiceNum == 2)
                {
                    cout<<"Performing Audio Reversal Now\n";
                    (*(wArr + activewhisp)).audReversal();
                    (*(wArr + activewhisp)).downld("reversed.wav");
                    cout<<"Wohoo! Go check the new file!\n";
                }
                else if (choiceNum == 3)
                {
                    cout << "Performing Normalisation of the Audio Now\n";
                    (*(wArr + activewhisp)).pNormal();
                    (*(wArr + activewhisp)).downld("normalized.wav");
                    cout << "Wohoo! Go check the new file!\n";

                }
                else if (choiceNum == 4)
                {
                    cout<<"Let's extract the chunk of the Audio Sample, shall we?\nEnter the starting time stap = ";
                    float s, e;
                    cin>>s;
                    cout<<"Enter the end time stamp = ";
                    cin>>e;
                    //creating a enew object to store the new chunk
                    Whisper cut = (*(wArr + activewhisp)).splice(s, e);
                    *(wArr + activewhisp) = cut;
                    (*(wArr + activewhisp)).downld("splice.wav");
                    cout << "Wohoo! Go check the new file!\n";
                }
            }
        }
        else if (choice == 2)
        {   
            if ((*(wArr + activewhisp)).numOfSamples == 0)
            {
                cout << "Oops! Seems like you have not selected [1]. Select that first to proceed..." << endl;
                continue;
            }
            int filtertype = 0;
            cout << "\n--- Welcome to the Frequency Domain Menu ---\n";
            cout << "Select from the three Filters to apply\n1. Low Pass Filter\n2. High Pass Filter\n3. Band Pass Filter\n";
            cout << "Choice: ";
            cin >> filtertype;
            while (cin.fail() || filtertype < 1 || filtertype > 3)
            {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid Input for file! Re-enter option 1, 2, 3, or 4: ";
                cin >> filtertype;
            }
            cout << "Decouplinggggg... wait a few seconds\n";
            *(sArr + activespect) = (*(wArr +activewhisp)).fwdTransform();
            if (filtertype == 1)
            {
                cout<<"Enter limit for the Low Pass Filter : ";
                float lim;
                cin>>lim;
                if (cin.fail()) 
                {
                    cin.clear(); cin.ignore(10000, '\n'); 
                    lim = 1000; 
                }
                //creating filter object and passing the padding size and sample rate value in hdr
                Filter filterOne((*(sArr + activespect)).pSize, (*(wArr+activewhisp)).hdr.sampleRate);
                //sending limit
                filterOne.lpFilter(lim);
                (*(sArr + activespect)).filteration(filterOne);
                //this for multiplying freq bins with filter weights
                //low freq * 1.0 and high cancel out
                WavHeader fH = (*(wArr + activewhisp)).hdr;
                Whisper filtr = (*(sArr + activespect)).i_FFT();
                filtr.hdr = fH;
                *(wArr + activewhisp) = filtr;
                (*(wArr + activewhisp)).downld("lowpass.wav");
                cout << "Wohoo! Go check the new file!\n";
            }
            else if (filtertype == 2)
            {
                cout << "Enter limit for the High Pass Filter : ";
                float lim;
                cin>>lim;
                Filter filterTwo((*(sArr + activespect)).pSize, (*(wArr + activewhisp)).hdr.sampleRate);
                filterTwo.hpFilter(lim);
                (*(sArr + activespect)).filteration(filterTwo);
                //this for multiplying freq bins with filter weights
                //low freq * 1.0 and high cancel out
                WavHeader fH = (*(wArr + activewhisp)).hdr;
                Whisper filtr = (*(sArr + activespect)).i_FFT();
                filtr.hdr = fH;
                *(wArr + activewhisp) = filtr;
                (*(wArr + activewhisp)).downld("highpass.wav");
                cout << "Wohoo! Go check the new file!\n";
            }
            else if (filtertype == 3)
            {
                cout<<"Enter the lower and higher limit for Band Pass filter: \n1. Lower: ";
                float lowlim;
                float highlim;
                cin>>lowlim;
                cout<<"\n2.Higher: ";
                cin>>highlim;
                if (cin.fail()) 
                {
                    cin.clear(); cin.ignore(10000, '\n'); lowlim = 500; highlim = 1500;
                }
                Filter filterThree((*(sArr + activespect)).pSize, (*(wArr + activewhisp)).hdr.sampleRate);
                filterThree.bpFilter(lowlim, highlim);
                (*(sArr + activespect)).filteration(filterThree);
                WavHeader fH = (*(wArr + activewhisp)).hdr;
                Whisper filtr = (*(sArr + activespect)).i_FFT();
                filtr.hdr = fH;
                *(wArr + activewhisp) = filtr;
                (*(wArr + activewhisp)).downld("bandpass.wav");
                cout << "Wohoo! Go check the new file!\n";
            }
        }
        else if (choice == 3)
        {
            cout<<"That's all for today fam!"<<endl;
            isRunning = false;
        }

    }
   
    return 0;
}