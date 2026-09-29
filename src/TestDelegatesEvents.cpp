#include <BNM/UserSettings/GlobalSettings.hpp>
#include <BNM/Delegates.hpp>
#include <BNM/Event.hpp>
#include <BNM/CustomEvent.hpp>
#include <BNM/UnityStructures.hpp>
#include <BNM/ClassesManagement.hpp>

struct TestClass {
    void MemberAction(int x, float y) {}
    int MemberFunc(bool b) const { return b ? 1 : 0; }
};

void TestHelperFunctions() {
    // 1. C# Action testing (stateless & stateful lambda)
    int captured = 42;
    auto act1 = BNM::CreateAction([]() {});
    auto act2 = BNM::CreateAction([captured](int x, float y) {
        (void)captured; (void)x; (void)y;
    });

    // 2. Member function pointer binding
    TestClass obj;
    auto actMember = BNM::CreateAction(&obj, &TestClass::MemberAction);
    auto funcMember = BNM::CreateFunc(&obj, &TestClass::MemberFunc);

    // 3. C# Func, Predicate, Comparison, EventHandler
    auto func1 = BNM::CreateFunc<int, float>([](float v) -> int { return (int)v; });
    auto funcAuto = BNM::CreateFunc([](int a, int b) -> int { return a + b; });
    auto pred = BNM::CreatePredicate<int>([](int x) { return x > 0; });
    auto comp = BNM::CreateComparison<float>([](float a, float b) { return a < b ? -1 : (a > b ? 1 : 0); });
    auto evtHandler = BNM::CreateEventHandler<int>([](BNM::IL2CPP::Il2CppObject *sender, int args) {});

    // 4. Custom C++ Event Dispatcher
    BNM::CustomEvent<void(int, float)> customEvt;
    auto id1 = customEvt += [](int a, float b) {};
    customEvt(10, 20.0f);
    customEvt -= id1;

    // 5. ScopedEventListener RAII Guard
    {
        BNM::ScopedEventListener guard([&]() {
            // cleanup logic
        });
        bool active = guard.IsActive();
        (void)active;
    }

    // 6. UnityAction & UnityEvent
    auto uAction0 = BNM::UnityEngine::CreateUnityAction([]() {});
    auto uAction1 = BNM::UnityEngine::CreateUnityAction([](int val) {});
    auto uActionMember = BNM::UnityEngine::CreateUnityAction(&obj, &TestClass::MemberAction);

    (void)act1; (void)act2; (void)actMember; (void)funcMember;
    (void)func1; (void)funcAuto; (void)pred; (void)comp; (void)evtHandler;
    (void)uAction0; (void)uAction1; (void)uActionMember;
}

void TestUnityCoreObjects() {
    using namespace BNM::UnityEngine;

    // 1. UnityEngine.Object
    Object *obj = nullptr;
    if (obj && obj->Alive()) {
        auto name = obj->GetName();
        obj->SetName("TestName");
        obj->SetName(name);
        int instId = obj->GetInstanceID();
        auto flags = obj->GetHideFlags();
        obj->SetHideFlags(flags);
        Object::Destroy(obj);
        Object::DestroyImmediate(obj);
        Object::DontDestroyOnLoad(obj);
        (void)instId;
    }

    // 2. UnityEngine.GameObject
    auto go = GameObject::Create("Player");
    if (go && go->Alive()) {
        go->SetActive(true);
        bool active = go->GetActiveSelf();
        int layer = go->GetLayer();
        go->SetLayer(layer);
        bool hasTag = go->CompareTag("Player");

        // 3-tier GetComponent / AddComponent / TryGetComponent
        auto comp1 = go->GetComponent<Component*>();
        auto comp2 = go->GetComponent("Transform", "UnityEngine");
        auto comp3 = go->GetComponent(BNM::Defaults::Get<Transform>().ToClass());

        Component *foundComp = nullptr;
        bool hasComp = go->TryGetComponent("Transform", foundComp);
        Transform *foundTr = nullptr;
        bool hasCompT = go->TryGetComponent(foundTr);

        go->SendMessage("OnDeath");
        go->SendMessageUpwards("OnScoreChanged");
        go->BroadcastMessage("OnPause");

        (void)active; (void)hasTag; (void)comp1; (void)comp2; (void)comp3; (void)hasComp; (void)hasCompT; (void)foundTr;
    }

    // 3. UnityEngine.Transform
    Transform *tr = nullptr;
    if (tr && tr->Alive()) {
        tr->SetPosition({1.f, 2.f, 3.f});
        auto pos = tr->GetPosition();
        tr->SetLocalPosition({0.f, 0.f, 0.f});
        auto lpos = tr->GetLocalPosition();
        tr->SetRotation(BNM::Structures::Unity::Quaternion());
        auto rot = tr->GetRotation();
        tr->SetEulerAngles({0.f, 90.f, 0.f});
        auto eul = tr->GetEulerAngles();
        tr->SetLocalScale({1.f, 1.f, 1.f});
        auto scale = tr->GetLocalScale();
        auto lossy = tr->GetLossyScale();

        auto fwd = tr->GetForward();
        auto up = tr->GetUp();
        auto right = tr->GetRight();

        auto parent = tr->GetParent();
        int childCount = tr->GetChildCount();
        auto child0 = tr->GetChild(0);
        auto childFind = tr->Find("Arm");

        tr->SetParent(parent, false);
        tr->DetachChildren();
        tr->SetAsFirstSibling();
        tr->SetAsLastSibling();
        int sibIdx = tr->GetSiblingIndex();
        tr->SetSiblingIndex(sibIdx);
        bool isChild = tr->IsChildOf(parent);

        tr->LookAt(parent);
        tr->Rotate({0.f, 45.f, 0.f});
        tr->Translate({0.f, 0.f, 1.f});
        tr->RotateAround({0.f, 0.f, 0.f}, {0.f, 1.f, 0.f}, 90.f);

        auto worldPt = tr->TransformPoint({1.f, 0.f, 0.f});
        auto localPt = tr->InverseTransformPoint(worldPt);
        auto l2w = tr->GetLocalToWorldMatrix();
        auto w2l = tr->GetWorldToLocalMatrix();

        (void)pos; (void)lpos; (void)rot; (void)eul; (void)scale; (void)lossy;
        (void)fwd; (void)up; (void)right; (void)childCount; (void)child0; (void)childFind;
        (void)isChild; (void)worldPt; (void)localPt; (void)l2w; (void)w2l;
    }

    // 4. UnityEngine.Behaviour & MonoBehaviour
    MonoBehaviour *mb = nullptr;
    if (mb && mb->Alive()) {
        mb->SetEnabled(true);
        bool en = mb->GetEnabled();
        bool activeAndEn = mb->GetIsActiveAndEnabled();

        mb->Invoke("DoSomething", 1.5f);
        mb->InvokeRepeating("Tick", 0.f, 0.1f);
        bool invoking = mb->IsInvoking("Tick");
        mb->CancelInvoke("Tick");
        mb->CancelInvoke();

        mb->StopAllCoroutines();

        (void)en; (void)activeAndEn; (void)invoking;
    }

    // 5. UnityEngine.ScriptableObject
    auto so = ScriptableObject::CreateInstance("MyScriptableAsset");
    (void)so;
}

void TestUnityApplicationAndRendering() {
    using namespace BNM::UnityEngine;
    using namespace BNM::Structures::Unity;

    // 1. Application
    auto dataPath = Application::GetDataPath();
    auto pDataPath = Application::GetPersistentDataPath();
    auto sDataPath = Application::GetStreamingAssetsPath();
    auto tDataPath = Application::GetTemporaryCachePath();
    auto id = Application::GetIdentifier();
    auto ver = Application::GetVersion();
    auto uVer = Application::GetUnityVersion();
    int fps = Application::GetTargetFrameRate();
    Application::SetTargetFrameRate(60);
    bool isPlaying = Application::GetIsPlaying();
    int sysLang = Application::GetSystemLanguage();
    int platform = Application::GetPlatform();
    Application::OpenURL("https://github.com");
    Application::Quit(0);

    (void)dataPath; (void)pDataPath; (void)sDataPath; (void)tDataPath;
    (void)id; (void)ver; (void)uVer; (void)fps; (void)isPlaying;
    (void)sysLang; (void)platform;

    // 2. Camera
    auto mainCam = Camera::GetMain();
    auto curCam = Camera::GetCurrent();
    auto allCams = Camera::GetAllCameras();
    int camCount = Camera::GetAllCamerasCount();
    (void)mainCam; (void)curCam; (void)allCams; (void)camCount;

    if (mainCam && mainCam->Alive()) {
        auto screenPt = mainCam->WorldToScreenPoint({1.f, 2.f, 3.f});
        auto worldPt = mainCam->ScreenToWorldPoint({100.f, 200.f, 5.f});
        auto ray3 = mainCam->ScreenPointToRay(Vector3{100.f, 200.f, 0.f});
        auto ray2 = mainCam->ScreenPointToRay(Vector2{100.f, 200.f});
        auto vpWorld = mainCam->ViewportToWorldPoint({0.5f, 0.5f, 5.f});
        auto worldVp = mainCam->WorldToViewportPoint({1.f, 2.f, 3.f});
        auto vpRay = mainCam->ViewportPointToRay({0.5f, 0.5f, 0.f});

        float fov = mainCam->GetFieldOfView();
        mainCam->SetFieldOfView(60.f);
        float nearP = mainCam->GetNearClipPlane();
        mainCam->SetNearClipPlane(0.1f);
        float farP = mainCam->GetFarClipPlane();
        mainCam->SetFarClipPlane(500.f);
        bool ortho = mainCam->GetOrthographic();
        mainCam->SetOrthographic(false);
        float orthoSize = mainCam->GetOrthographicSize();
        mainCam->SetOrthographicSize(5.f);
        float depth = mainCam->GetDepth();
        mainCam->SetDepth(0.f);
        int mask = mainCam->GetCullingMask();
        mainCam->SetCullingMask(-1);
        auto targetTex = mainCam->GetTargetTexture();
        mainCam->SetTargetTexture(nullptr);
        int pxW = mainCam->GetPixelWidth();
        int pxH = mainCam->GetPixelHeight();
        auto pxRect = mainCam->GetPixelRect();
        auto rect = mainCam->GetRect();
        float aspect = mainCam->GetAspect();
        auto bgColor = mainCam->GetBackgroundColor();
        mainCam->SetBackgroundColor(Color{0.f, 0.f, 0.f, 1.f});

        (void)screenPt; (void)worldPt; (void)ray3; (void)ray2; (void)vpWorld;
        (void)worldVp; (void)vpRay; (void)fov; (void)nearP; (void)farP;
        (void)ortho; (void)orthoSize; (void)depth; (void)mask; (void)targetTex;
        (void)pxW; (void)pxH; (void)pxRect; (void)rect; (void)aspect; (void)bgColor;
    }

    // 3. Light
    Light *light = nullptr;
    if (light && light->Alive()) {
        auto col = light->GetColor();
        light->SetColor(Color{1.f, 1.f, 1.f, 1.f});
        float inten = light->GetIntensity();
        light->SetIntensity(1.5f);
        float bounce = light->GetBounceIntensity();
        light->SetBounceIntensity(1.f);
        float range = light->GetRange();
        light->SetRange(10.f);
        int lType = light->GetType();
        light->SetType(1);
        int shadows = light->GetShadows();
        light->SetShadows(2);
        float spotAngle = light->GetSpotAngle();
        light->SetSpotAngle(45.f);

        (void)col; (void)inten; (void)bounce; (void)range; (void)lType; (void)shadows; (void)spotAngle;
    }

    // 4. Shader & Material
    auto shader = Shader::Find("Standard");
    int propId = Shader::PropertyToID("_MainTex");
    bool supp = false;
    if (shader && shader->Alive()) {
        supp = shader->GetIsSupported();
        shader->EnableKeyword("TEST_KEYWORD");
        shader->DisableKeyword("TEST_KEYWORD");
    }
    (void)propId; (void)supp;

    auto mat = Material::Create(shader);
    if (mat && mat->Alive()) {
        auto s = mat->GetShader();
        mat->SetShader(shader);
        auto col = mat->GetColor();
        mat->SetColor(Color{1.f, 0.f, 0.f, 1.f});
        mat->SetColor("_SpecColor", Color{0.5f, 0.5f, 0.5f, 1.f});
        auto colNamed = mat->GetColor("_SpecColor");
        auto tex = mat->GetMainTexture();
        mat->SetMainTexture(nullptr);
        auto texOff = mat->GetMainTextureOffset();
        mat->SetMainTextureOffset({0.f, 0.f});
        auto texScale = mat->GetMainTextureScale();
        mat->SetMainTextureScale({1.f, 1.f});
        int q = mat->GetRenderQueue();
        mat->SetRenderQueue(2000);
        mat->SetFloat("_Glossiness", 0.5f);
        float gloss = mat->GetFloat("_Glossiness");
        mat->SetInt("_SrcBlend", 1);
        int blend = mat->GetInt("_SrcBlend");
        mat->SetVector("_Center", Vector4{0.f, 0.f, 0.f, 0.f});
        auto vec = mat->GetVector("_Center");
        mat->EnableKeyword("_ALPHABLEND_ON");
        mat->DisableKeyword("_ALPHABLEND_ON");
        bool kwEn = mat->IsKeywordEnabled("_ALPHABLEND_ON");
        bool hasProp = mat->HasProperty("_MainTex");

        (void)s; (void)col; (void)colNamed; (void)tex; (void)texOff; (void)texScale;
        (void)q; (void)gloss; (void)blend; (void)vec; (void)kwEn; (void)hasProp;
    }

    // 5. Texture, Texture2D, RenderTexture
    auto tex2D = Texture2D::Create(256, 256);
    if (tex2D && tex2D->Alive()) {
        int w = tex2D->GetWidth();
        int h = tex2D->GetHeight();
        int dim = tex2D->GetDimension();
        int filter = tex2D->GetFilterMode();
        tex2D->SetFilterMode(1);
        int wrap = tex2D->GetWrapMode();
        tex2D->SetWrapMode(0);
        int aniso = tex2D->GetAnisoLevel();
        tex2D->SetAnisoLevel(2);
        void *nativePtr = tex2D->GetNativeTexturePtr();

        auto px = tex2D->GetPixel(0, 0);
        tex2D->SetPixel(0, 0, Color{1.f, 1.f, 1.f, 1.f});
        auto pxBilinear = tex2D->GetPixelBilinear(0.5f, 0.5f);
        tex2D->Apply(true, false);
        bool resized = tex2D->Resize(512, 512);
        tex2D->ReadPixels(Rect{0.f, 0.f, 512.f, 512.f}, 0, 0);
        auto pngBytes = tex2D->EncodeToPNG();
        auto jpgBytes = tex2D->EncodeToJPG(85);

        (void)w; (void)h; (void)dim; (void)filter; (void)wrap; (void)aniso; (void)nativePtr;
        (void)px; (void)pxBilinear; (void)resized; (void)pngBytes; (void)jpgBytes;
    }

    auto rt = RenderTexture::GetTemporary(512, 512, 24);
    if (rt && rt->Alive()) {
        int depth = rt->GetDepth();
        rt->SetDepth(24);
        bool created = rt->IsCreated();
        bool ok = rt->Create();
        rt->Release();
        RenderTexture::ReleaseTemporary(rt);

        (void)depth; (void)created; (void)ok;
    }
}

#ifdef BNM_UNITY_PHYSICS
void TestUnityPhysics() {
    using namespace BNM::UnityEngine;
    using namespace BNM::Structures::Unity;

    // Rigidbody
    Rigidbody *rb = nullptr;
    if (rb && rb->Alive()) {
        auto vel = rb->GetVelocity();
        rb->SetVelocity({0.f, 5.f, 0.f});
        auto angVel = rb->GetAngularVelocity();
        rb->SetAngularVelocity({0.f, 1.f, 0.f});
        float mass = rb->GetMass();
        rb->SetMass(10.f);
        float drag = rb->GetDrag();
        rb->SetDrag(0.5f);
        float angDrag = rb->GetAngularDrag();
        rb->SetAngularDrag(0.05f);
        bool grav = rb->GetUseGravity();
        rb->SetUseGravity(true);
        bool kin = rb->GetIsKinematic();
        rb->SetIsKinematic(false);
        bool freezeRot = rb->GetFreezeRotation();
        rb->SetFreezeRotation(true);
        int constr = rb->GetConstraints();
        rb->SetConstraints(0);
        auto pos = rb->GetPosition();
        rb->SetPosition({1.f, 2.f, 3.f});
        auto rot = rb->GetRotation();
        rb->SetRotation(Quaternion{0.f, 0.f, 0.f, 1.f});

        rb->AddForce({0.f, 10.f, 0.f});
        rb->AddRelativeForce({0.f, 10.f, 0.f});
        rb->AddTorque({0.f, 5.f, 0.f});
        rb->AddRelativeTorque({0.f, 5.f, 0.f});
        rb->AddForceAtPosition({0.f, 10.f, 0.f}, {1.f, 0.f, 0.f});
        rb->AddExplosionForce(100.f, {0.f, 0.f, 0.f}, 5.f);
        rb->MovePosition({2.f, 3.f, 4.f});
        rb->MoveRotation(Quaternion{0.f, 0.f, 0.f, 1.f});
        rb->Sleep();
        bool isSleeping = rb->IsSleeping();
        rb->WakeUp();

        (void)vel; (void)angVel; (void)mass; (void)drag; (void)angDrag; (void)grav;
        (void)kin; (void)freezeRot; (void)constr; (void)pos; (void)rot; (void)isSleeping;
    }

    // Colliders
    BoxCollider *box = nullptr;
    if (box && box->Alive()) {
        auto center = box->GetCenter();
        box->SetCenter({0.f, 0.5f, 0.f});
        auto size = box->GetSize();
        box->SetSize({1.f, 1.f, 1.f});
        bool trig = box->GetIsTrigger();
        box->SetIsTrigger(false);
        auto closePt = box->ClosestPoint({5.f, 5.f, 5.f});

        (void)center; (void)size; (void)trig; (void)closePt;
    }

    SphereCollider *sphere = nullptr;
    if (sphere && sphere->Alive()) {
        auto center = sphere->GetCenter();
        sphere->SetCenter({0.f, 0.f, 0.f});
        float rad = sphere->GetRadius();
        sphere->SetRadius(2.f);
        (void)center; (void)rad;
    }

    CapsuleCollider *cap = nullptr;
    if (cap && cap->Alive()) {
        auto center = cap->GetCenter();
        cap->SetCenter({0.f, 1.f, 0.f});
        float rad = cap->GetRadius();
        cap->SetRadius(0.5f);
        float h = cap->GetHeight();
        cap->SetHeight(2.f);
        int dir = cap->GetDirection();
        cap->SetDirection(1);
        (void)center; (void)rad; (void)h; (void)dir;
    }

    MeshCollider *meshCol = nullptr;
    if (meshCol && meshCol->Alive()) {
        bool cvx = meshCol->GetConvex();
        meshCol->SetConvex(true);
        (void)cvx;
    }

    // Static Physics API
    RaycastHit hit;
    bool hitOk = Physics::Raycast({0.f, 10.f, 0.f}, {0.f, -1.f, 0.f}, hit, 100.f, -1);
    bool lineHit = Physics::Linecast({0.f, 10.f, 0.f}, {0.f, 0.f, 0.f}, hit);
    bool checkSph = Physics::CheckSphere({0.f, 0.f, 0.f}, 5.f, -1);
    bool checkBx = Physics::CheckBox({0.f, 0.f, 0.f}, {1.f, 1.f, 1.f});
    auto grav = Physics::GetGravity();
    Physics::SetGravity({0.f, -9.81f, 0.f});
    Physics::IgnoreLayerCollision(0, 1, true);
    bool ign = Physics::GetIgnoreLayerCollision(0, 1);
    Physics::SyncTransforms();

    (void)hitOk; (void)lineHit; (void)checkSph; (void)checkBx; (void)grav; (void)ign;
}
#endif

#ifdef BNM_UNITY_PHYSICS2D
void TestUnityPhysics2D() {
    using namespace BNM::UnityEngine;
    using namespace BNM::Structures::Unity;

    // Rigidbody2D
    Rigidbody2D *rb2d = nullptr;
    if (rb2d && rb2d->Alive()) {
        auto pos = rb2d->GetPosition();
        rb2d->SetPosition({1.f, 2.f});
        float rot = rb2d->GetRotation();
        rb2d->SetRotation(45.f);
        auto vel = rb2d->GetVelocity();
        rb2d->SetVelocity({5.f, 0.f});
        float angVel = rb2d->GetAngularVelocity();
        rb2d->SetAngularVelocity(10.f);
        float mass = rb2d->GetMass();
        rb2d->SetMass(2.f);
        float gravScale = rb2d->GetGravityScale();
        rb2d->SetGravityScale(1.f);
        int bType = rb2d->GetBodyType();
        rb2d->SetBodyType(0);
        bool sim = rb2d->GetSimulated();
        rb2d->SetSimulated(true);

        rb2d->AddForce({10.f, 0.f});
        rb2d->AddRelativeForce({0.f, 10.f});
        rb2d->AddTorque(5.f);
        rb2d->AddForceAtPosition({0.f, 10.f}, {1.f, 1.f});
        rb2d->MovePosition({3.f, 4.f});
        rb2d->MoveRotation(90.f);
        rb2d->Sleep();
        bool isSleeping = rb2d->IsSleeping();
        rb2d->WakeUp();

        (void)pos; (void)rot; (void)vel; (void)angVel; (void)mass; (void)gravScale;
        (void)bType; (void)sim; (void)isSleeping;
    }

    // BoxCollider2D & CircleCollider2D
    BoxCollider2D *box2d = nullptr;
    if (box2d && box2d->Alive()) {
        auto sz = box2d->GetSize();
        box2d->SetSize({2.f, 2.f});
        auto off = box2d->GetOffset();
        box2d->SetOffset({0.f, 0.f});
        float edgeRad = box2d->GetEdgeRadius();
        box2d->SetEdgeRadius(0.1f);
        bool trig = box2d->GetIsTrigger();
        box2d->SetIsTrigger(false);
        (void)sz; (void)off; (void)edgeRad; (void)trig;
    }

    CircleCollider2D *circ2d = nullptr;
    if (circ2d && circ2d->Alive()) {
        float r = circ2d->GetRadius();
        circ2d->SetRadius(1.5f);
        (void)r;
    }

    // Physics2D static methods
    auto hit2d = Physics2D::Raycast({0.f, 5.f}, {0.f, -1.f}, 10.f, -1);
    auto line2d = Physics2D::Linecast({0.f, 5.f}, {0.f, 0.f});
    auto ovPoint = Physics2D::OverlapPoint({0.f, 0.f});
    auto ovCirc = Physics2D::OverlapCircle({0.f, 0.f}, 2.f);
    auto ovBox = Physics2D::OverlapBox({0.f, 0.f}, {2.f, 2.f}, 0.f);
    auto grav2d = Physics2D::GetGravity();
    Physics2D::SetGravity({0.f, -9.81f});
    Physics2D::IgnoreLayerCollision(0, 1, true);
    bool ign2d = Physics2D::GetIgnoreLayerCollision(0, 1);
    Physics2D::SyncTransforms();

    (void)hit2d; (void)line2d; (void)ovPoint; (void)ovCirc; (void)ovBox; (void)grav2d; (void)ign2d;
}
#endif

#ifdef BNM_UNITY_UI
void TestUnityUI() {
    using namespace BNM::UnityEngine::UI;
    using namespace BNM::Structures::Unity;

    // RectTransform
    RectTransform *rt = nullptr;
    if (rt && rt->Alive()) {
        auto anchPos = rt->GetAnchoredPosition();
        rt->SetAnchoredPosition({10.f, 20.f});
        auto anchPos3D = rt->GetAnchoredPosition3D();
        rt->SetAnchoredPosition3D({10.f, 20.f, 0.f});
        auto sizeDelta = rt->GetSizeDelta();
        rt->SetSizeDelta({100.f, 50.f});
        auto anchMin = rt->GetAnchorMin();
        rt->SetAnchorMin({0.f, 0.f});
        auto anchMax = rt->GetAnchorMax();
        rt->SetAnchorMax({1.f, 1.f});
        auto pivot = rt->GetPivot();
        rt->SetPivot({0.5f, 0.5f});
        auto rect = rt->GetRect();
        rt->SetSizeWithCurrentAnchors(0, 200.f);
        rt->ForceUpdateRectTransforms();

        (void)anchPos; (void)anchPos3D; (void)sizeDelta; (void)anchMin; (void)anchMax; (void)pivot; (void)rect;
    }

    // Canvas & CanvasScaler
    Canvas *canvas = nullptr;
    if (canvas && canvas->Alive()) {
        int rMode = canvas->GetRenderMode();
        canvas->SetRenderMode(0);
        bool isRoot = canvas->GetIsRootCanvas();
        int order = canvas->GetSortingOrder();
        canvas->SetSortingOrder(10);
        float scale = canvas->GetScaleFactor();
        canvas->SetScaleFactor(1.5f);
        Canvas::ForceUpdateCanvases();

        (void)rMode; (void)isRoot; (void)order; (void)scale;
    }

    CanvasScaler *scaler = nullptr;
    if (scaler && scaler->Alive()) {
        int mode = scaler->GetUiScaleMode();
        scaler->SetUiScaleMode(1);
        auto refRes = scaler->GetReferenceResolution();
        scaler->SetReferenceResolution({1920.f, 108.f});
        float match = scaler->GetMatchWidthOrHeight();
        scaler->SetMatchWidthOrHeight(0.5f);

        (void)mode; (void)refRes; (void)match;
    }

    // Text & Image
    Text *txt = nullptr;
    if (txt && txt->Alive()) {
        auto tStr = txt->GetText();
        txt->SetText("Hello BNM");
        int fSize = txt->GetFontSize();
        txt->SetFontSize(24);
        int align = txt->GetAlignment();
        txt->SetAlignment(4);
        bool rich = txt->GetSupportRichText();
        txt->SetSupportRichText(true);
        float pW = txt->GetPreferredWidth();
        float pH = txt->GetPreferredHeight();

        (void)tStr; (void)fSize; (void)align; (void)rich; (void)pW; (void)pH;
    }

    Image *img = nullptr;
    if (img && img->Alive()) {
        int imgType = img->GetType();
        img->SetType(0);
        bool pAspect = img->GetPreserveAspect();
        img->SetPreserveAspect(true);
        float fill = img->GetFillAmount();
        img->SetFillAmount(0.75f);
        img->SetNativeSize();

        (void)imgType; (void)pAspect; (void)fill;
    }

    // Button, Slider, Toggle, InputField
    Button *btn = nullptr;
    if (btn && btn->Alive()) {
        bool inter = btn->GetInteractable();
        btn->SetInteractable(true);
        auto onClick = btn->GetOnClick();
        if (onClick) {
            onClick->AddListener([]() {});
        }
        (void)inter;
    }

    Slider *slider = nullptr;
    if (slider && slider->Alive()) {
        float val = slider->GetValue();
        slider->SetValue(0.5f);
        slider->SetValueWithoutNotify(0.8f);
        float minV = slider->GetMinValue();
        slider->SetMinValue(0.f);
        float maxV = slider->GetMaxValue();
        slider->SetMaxValue(1.f);
        bool whole = slider->GetWholeNumbers();
        slider->SetWholeNumbers(false);

        (void)val; (void)minV; (void)maxV; (void)whole;
    }

    Toggle *toggle = nullptr;
    if (toggle && toggle->Alive()) {
        bool on = toggle->GetIsOn();
        toggle->SetIsOn(true);
        toggle->SetIsOnWithoutNotify(false);
        (void)on;
    }

    InputField *input = nullptr;
    if (input && input->Alive()) {
        auto iText = input->GetText();
        input->SetText("New input text");
        input->SetTextWithoutNotify("Silent text");
        int charLim = input->GetCharacterLimit();
        input->SetCharacterLimit(50);
        bool ro = input->GetReadOnly();
        input->SetReadOnly(false);
        bool foc = input->GetIsFocused();
        input->ActivateInputField();
        input->DeactivateInputField();

        (void)iText; (void)charLim; (void)ro; (void)foc;
    }
}
#endif

#ifdef BNM_UNITY_TEXTMESHPRO
void TestUnityTextMeshPro() {
    using namespace BNM::UnityEngine::TMPro;
    using namespace BNM::Structures::Unity;

    TMP_Text *tmpText = nullptr;
    if (tmpText && tmpText->Alive()) {
        auto t = tmpText->GetText();
        tmpText->SetText("TextMeshPro Text");
        float fSize = tmpText->GetFontSize();
        tmpText->SetFontSize(32.f);
        int align = tmpText->GetAlignment();
        tmpText->SetAlignment(2);
        float alpha = tmpText->GetAlpha();
        tmpText->SetAlpha(0.9f);
        bool rich = tmpText->GetRichText();
        tmpText->SetRichText(true);
        auto margin = tmpText->GetMargin();
        tmpText->SetMargin({0.f, 0.f, 0.f, 0.f});
        bool wrap = tmpText->GetEnableWordWrapping();
        tmpText->SetEnableWordWrapping(true);
        int ov = tmpText->GetOverflowMode();
        tmpText->SetOverflowMode(1);
        int maxChar = tmpText->GetMaxVisibleCharacters();
        tmpText->SetMaxVisibleCharacters(100);
        tmpText->ForceMeshUpdate(true, true);
        tmpText->ClearMesh(true);

        (void)t; (void)fSize; (void)align; (void)alpha; (void)rich; (void)margin;
        (void)wrap; (void)ov; (void)maxChar;
    }

    TextMeshPro *tmp3d = nullptr;
    if (tmp3d && tmp3d->Alive()) {
        int sortLayer = tmp3d->GetSortingLayerID();
        tmp3d->SetSortingLayerID(0);
        int sortOrder = tmp3d->GetSortingOrder();
        tmp3d->SetSortingOrder(1);
        (void)sortLayer; (void)sortOrder;
    }

    TextMeshProUGUI *tmpUGUI = nullptr;
    (void)tmpUGUI;

    TMP_InputField *tmpInput = nullptr;
    if (tmpInput && tmpInput->Alive()) {
        auto inpText = tmpInput->GetText();
        tmpInput->SetText("TMP Input");
        tmpInput->SetTextWithoutNotify("TMP Silent");
        float ptSize = tmpInput->GetPointSize();
        tmpInput->SetPointSize(16.f);
        bool isFoc = tmpInput->GetIsFocused();
        tmpInput->ActivateInputField();
        tmpInput->DeactivateInputField();

        (void)inpText; (void)ptSize; (void)isFoc;
    }
}
#endif

void TestUnityCoreUtils() {
    using namespace BNM::UnityEngine;
    using namespace BNM::Structures::Unity;

    // 1. Time (Speedhack & Frame Timing)
    float dt = Time::GetDeltaTime();
    float fdt = Time::GetFixedDeltaTime();
    Time::SetFixedDeltaTime(0.02f);
    float t = Time::GetTime();
    float tLoad = Time::GetTimeSinceLevelLoad();
    float ts = Time::GetTimeScale();
    Time::SetTimeScale(2.0f); // 2x Speedhack test
    float rt = Time::GetRealtimeSinceStartup();
    float udt = Time::GetUnscaledDeltaTime();
    float ut = Time::GetUnscaledTime();
    int fc = Time::GetFrameCount();
    float maxDt = Time::GetMaximumDeltaTime();
    Time::SetMaximumDeltaTime(0.333f);
    float sdt = Time::GetSmoothDeltaTime();
    int capFps = Time::GetCaptureFramerate();
    Time::SetCaptureFramerate(60);

    (void)dt; (void)fdt; (void)t; (void)tLoad; (void)ts; (void)rt; (void)udt; (void)ut; (void)fc; (void)maxDt; (void)sdt; (void)capFps;

    // 2. Screen (Display & Touch Metrics)
    int sw = Screen::GetWidth();
    int sh = Screen::GetHeight();
    float dpi = Screen::GetDpi();
    int orient = Screen::GetOrientation();
    Screen::SetOrientation(orient);
    int sleep = Screen::GetSleepTimeout();
    Screen::SetSleepTimeout(sleep);
    BNM::Structures::Unity::Rect safeArea = Screen::GetSafeArea();
    bool fs = Screen::GetFullScreen();
    Screen::SetFullScreen(true);
    Screen::SetResolution(1920, 1080, true);
    float bright = Screen::GetBrightness();
    Screen::SetBrightness(1.0f);
    auto cutouts = Screen::GetCutouts();

    (void)sw; (void)sh; (void)dpi; (void)orient; (void)sleep; (void)safeArea; (void)fs; (void)bright; (void)cutouts;

    // 3. Input & Touch
    bool kDown = Input::GetKeyDown(119); // 'w'
    bool kUp = Input::GetKeyUp(119);
    bool kHold = Input::GetKey(119);
    bool m0 = Input::GetMouseButton(0);
    bool m0d = Input::GetMouseButtonDown(0);
    bool m0u = Input::GetMouseButtonUp(0);
    Vector3 mPos = Input::GetMousePosition();
    Vector2 mScroll = Input::GetMouseScrollDelta();
    float hAxis = Input::GetAxis("Horizontal");
    float vAxisRaw = Input::GetAxisRaw("Vertical");
    bool btn = Input::GetButton("Fire1");
    bool btnD = Input::GetButtonDown("Fire1");
    bool btnU = Input::GetButtonUp("Fire1");
    int tCount = Input::GetTouchCount();
    Touch t0 = Input::GetTouch(0);
    auto touches = Input::GetTouches();
    bool tSupp = Input::GetTouchSupported();
    bool mTouch = Input::GetMultiTouchEnabled();
    Input::SetMultiTouchEnabled(true);
    Vector3 accel = Input::GetAcceleration();
    auto loc = Input::GetLocation();
    auto gyro = Input::GetGyro();
    auto comp = Input::GetCompass();

    int fid = t0.GetFingerId();
    Vector2 tPos = t0.GetPosition();
    Vector2 tDelta = t0.GetDeltaPosition();
    float tDt = t0.GetDeltaTime();
    int tapC = t0.GetTapCount();
    int phase = t0.GetPhase();
    float press = t0.GetPressure();
    float maxPress = t0.GetMaximumPossiblePressure();
    int tType = t0.GetType();
    float alt = t0.GetAltitudeAngle();
    float azm = t0.GetAzimuthAngle();
    float rad = t0.GetRadius();
    float radVar = t0.GetRadiusVariance();

    (void)kDown; (void)kUp; (void)kHold; (void)m0; (void)m0d; (void)m0u; (void)mPos; (void)mScroll;
    (void)hAxis; (void)vAxisRaw; (void)btn; (void)btnD; (void)btnU; (void)tCount; (void)touches; (void)tSupp; (void)mTouch;
    (void)accel; (void)loc; (void)gyro; (void)comp;
    (void)fid; (void)tPos; (void)tDelta; (void)tDt; (void)tapC; (void)phase; (void)press; (void)maxPress; (void)tType; (void)alt; (void)azm; (void)rad; (void)radVar;

    // 4. SystemInfo (Device & GPU Inspection)
    auto uid = SystemInfo::GetDeviceUniqueIdentifier();
    auto dName = SystemInfo::GetDeviceName();
    auto dModel = SystemInfo::GetDeviceModel();
    auto dType = SystemInfo::GetDeviceType();
    auto os = SystemInfo::GetOperatingSystem();
    auto osFam = SystemInfo::GetOperatingSystemFamily();
    auto cpu = SystemInfo::GetProcessorType();
    int cpuCount = SystemInfo::GetProcessorCount();
    int cpuFreq = SystemInfo::GetProcessorFrequencyMHz();
    int ram = SystemInfo::GetSystemMemorySizeMB();
    auto gpu = SystemInfo::GetGraphicsDeviceName();
    auto gpuVen = SystemInfo::GetGraphicsDeviceVendor();
    int vram = SystemInfo::GetGraphicsMemorySizeMB();
    auto gpuType = SystemInfo::GetGraphicsDeviceType();
    int shaderLvl = SystemInfo::GetGraphicsShaderLevel();
    int maxTex = SystemInfo::GetMaxTextureSize();
    bool shd = SystemInfo::GetSupportsShadows();
    bool tex3d = SystemInfo::GetSupports3DTextures();
    bool cs = SystemInfo::GetSupportsComputeShaders();
    bool inst = SystemInfo::GetSupportsInstancing();
    float bat = SystemInfo::GetBatteryLevel();
    int batStat = SystemInfo::GetBatteryStatus();

    (void)uid; (void)dName; (void)dModel; (void)dType; (void)os; (void)osFam; (void)cpu; (void)cpuCount; (void)cpuFreq; (void)ram;
    (void)gpu; (void)gpuVen; (void)vram; (void)gpuType; (void)shaderLvl; (void)maxTex; (void)shd; (void)tex3d; (void)cs; (void)inst; (void)bat; (void)batStat;

    // 5. PlayerPrefs
    PlayerPrefs::SetInt("HighScore", 1000);
    int hs = PlayerPrefs::GetInt("HighScore", 0);
    PlayerPrefs::SetFloat("Volume", 0.8f);
    float vol = PlayerPrefs::GetFloat("Volume", 1.0f);
    PlayerPrefs::SetString("Username", "Cheater");
    auto user = PlayerPrefs::GetString("Username", "Guest");
    bool hasHs = PlayerPrefs::HasKey("HighScore");
    PlayerPrefs::DeleteKey("HighScore");
    PlayerPrefs::Save();

    (void)hs; (void)vol; (void)user; (void)hasHs;

    // 6. Bounds
    Bounds b({0.f, 0.f, 0.f}, {2.f, 2.f, 2.f});
    auto bCenter = b.GetCenter();
    b.SetCenter({1.f, 1.f, 1.f});
    auto bSize = b.GetSize();
    b.SetSize({4.f, 4.f, 4.f});
    auto bExtents = b.GetExtents();
    b.SetExtents({2.f, 2.f, 2.f});
    auto bMin = b.GetMin();
    b.SetMin({-1.f, -1.f, -1.f});
    auto bMax = b.GetMax();
    b.SetMax({3.f, 3.f, 3.f});
    b.Encapsulate(Vector3{5.f, 5.f, 5.f});
    b.Encapsulate(Bounds({0.f, 0.f, 0.f}, {1.f, 1.f, 1.f}));
    bool bContains = b.Contains({1.f, 1.f, 1.f});
    bool bIntersects = b.Intersects(Bounds({0.f, 0.f, 0.f}, {2.f, 2.f, 2.f}));
    Vector3 bClose = b.ClosestPoint({10.f, 10.f, 10.f});

    (void)bCenter; (void)bSize; (void)bExtents; (void)bMin; (void)bMax; (void)bContains; (void)bIntersects; (void)bClose;

    // 7. Defaults::Unbox<T>
    BNM::IL2CPP::Il2CppObject *boxedObj = nullptr;
    int unboxedInt = BNM::Defaults::Unbox<int>(boxedObj);
    float unboxedFloat = BNM::Defaults::Unbox<float>(boxedObj);
    Vector3 unboxedVec = BNM::Defaults::Unbox<Vector3>(boxedObj);
    (void)unboxedInt; (void)unboxedFloat; (void)unboxedVec;
}

void TestUnitySceneAndResources() {
    using namespace BNM::UnityEngine;
    using namespace BNM::UnityEngine::SceneManagement;

    // 1. SceneManager & Scene
    Scene activeScene = SceneManager::GetActiveScene();
    bool activeOk = SceneManager::SetActiveScene(activeScene);
    Scene scName = SceneManager::GetSceneByName("GameScene");
    Scene scPath = SceneManager::GetSceneByPath("Assets/Scenes/GameScene.unity");
    Scene scIdx = SceneManager::GetSceneByBuildIndex(0);
    Scene scAt = SceneManager::GetSceneAt(0);
    int scCount = SceneManager::GetSceneCount();
    int scBuildCount = SceneManager::GetSceneCountInBuildSettings();

    SceneManager::LoadScene("GameScene", 0);
    SceneManager::LoadScene(0, 1);
    auto asyncOp1 = SceneManager::LoadSceneAsync("GameScene", 0);
    auto asyncOp2 = SceneManager::LoadSceneAsync(0, 0);
    auto asyncOp3 = SceneManager::UnloadSceneAsync("GameScene");
    auto asyncOp4 = SceneManager::UnloadSceneAsync(activeScene);
    Scene newScene = SceneManager::CreateScene("GeneratedScene");
    SceneManager::MergeScenes(newScene, activeScene);

    bool scValid = activeScene.IsValid();
    auto scNameStr = activeScene.GetName();
    auto scPathStr = activeScene.GetPath();
    int scBuildIdx = activeScene.GetBuildIndex();
    bool scLoaded = activeScene.GetIsLoaded();
    bool scIsValid = activeScene.GetIsValid();
    bool scDirty = activeScene.GetIsDirty();
    int scRootC = activeScene.GetRootCount();
    auto rootObjs = activeScene.GetRootGameObjects();

    (void)activeOk; (void)scName; (void)scPath; (void)scIdx; (void)scAt; (void)scCount; (void)scBuildCount;
    (void)asyncOp1; (void)asyncOp2; (void)asyncOp3; (void)asyncOp4; (void)newScene;
    (void)scValid; (void)scNameStr; (void)scPathStr; (void)scBuildIdx; (void)scLoaded; (void)scIsValid; (void)scDirty; (void)scRootC; (void)rootObjs;

    // 2. AsyncOperation & YieldInstruction
    if (asyncOp1 && asyncOp1->Alive()) {
        bool done = asyncOp1->GetIsDone();
        float prog = asyncOp1->GetProgress();
        int prio = asyncOp1->GetPriority();
        asyncOp1->SetPriority(1);
        bool allow = asyncOp1->GetAllowSceneActivation();
        asyncOp1->SetAllowSceneActivation(true);

        (void)done; (void)prog; (void)prio; (void)allow;
    }

    // 3. Resources
    auto loadedObj = Resources::Load<Object*>("PlayerPrefab");
    auto loadedAll = Resources::LoadAll<Object*>("Textures");
    auto asyncRes = Resources::LoadAsync<Object*>("HeavyAsset");
    auto foundAll = Resources::FindObjectsOfTypeAll<Object*>();
    Resources::UnloadAsset(loadedObj);
    auto unloadOp = Resources::UnloadUnusedAssets();

    (void)loadedObj; (void)loadedAll; (void)asyncRes; (void)foundAll; (void)unloadOp;
}

#ifdef BNM_UNITY_RENDERERS
void TestUnityRenderers() {
    using namespace BNM::UnityEngine;
    using namespace BNM::Structures::Unity;

    // 1. Renderer
    Renderer *rend = nullptr;
    if (rend && rend->Alive()) {
        auto mat = rend->GetMaterial();
        rend->SetMaterial(mat);
        auto mats = rend->GetMaterials();
        rend->SetMaterials(mats);
        auto sMat = rend->GetSharedMaterial();
        rend->SetSharedMaterial(sMat);
        auto sMats = rend->GetSharedMaterials();
        rend->SetSharedMaterials(sMats);
        Bounds b = rend->GetBounds();
        bool en = rend->GetEnabled();
        rend->SetEnabled(true);
        bool vis = rend->GetIsVisible();
        int shadowMode = rend->GetShadowCastingMode();
        rend->SetShadowCastingMode(1);
        bool recShadow = rend->GetReceiveShadows();
        rend->SetReceiveShadows(true);
        int sLayer = rend->GetSortingLayerID();
        rend->SetSortingLayerID(0);
        int sOrder = rend->GetSortingOrder();
        rend->SetSortingOrder(1);

        (void)mat; (void)mats; (void)sMat; (void)sMats; (void)b; (void)en; (void)vis;
        (void)shadowMode; (void)recShadow; (void)sLayer; (void)sOrder;
    }

    // 2. MeshRenderer
    MeshRenderer *mr = nullptr;
    (void)mr;

    // 3. SkinnedMeshRenderer (Bone extraction for ESP/Aimbot)
    SkinnedMeshRenderer *smr = nullptr;
    if (smr && smr->Alive()) {
        auto bones = smr->GetBones(); // Essential for ESP Skeleton
        smr->SetBones(bones);
        auto rootBone = smr->GetRootBone();
        smr->SetRootBone(rootBone);
        auto sharedMesh = smr->GetSharedMesh();
        smr->SetSharedMesh(sharedMesh);
        int qual = smr->GetQuality();
        smr->SetQuality(2);
        bool offscreen = smr->GetUpdateWhenOffscreen();
        smr->SetUpdateWhenOffscreen(true);
        float w0 = smr->GetBlendShapeWeight(0);
        smr->SetBlendShapeWeight(0, 50.0f);
        Mesh *bakedMesh = Mesh::Create();
        smr->BakeMesh(bakedMesh);

        (void)bones; (void)rootBone; (void)sharedMesh; (void)qual; (void)offscreen; (void)w0; (void)bakedMesh;
    }

    // 4. SpriteRenderer
    SpriteRenderer *sr = nullptr;
    if (sr && sr->Alive()) {
        auto sprite = sr->GetSprite();
        sr->SetSprite(sprite);
        Color col = sr->GetColor();
        sr->SetColor(Color{1.f, 1.f, 1.f, 1.f});
        bool fx = sr->GetFlipX();
        sr->SetFlipX(false);
        bool fy = sr->GetFlipY();
        sr->SetFlipY(false);
        int dMode = sr->GetDrawMode();
        sr->SetDrawMode(0);
        Vector2 sz = sr->GetSize();
        sr->SetSize(sz);

        (void)sprite; (void)col; (void)fx; (void)fy; (void)dMode; (void)sz;
    }

    // 5. Mesh
    auto mesh = Mesh::Create();
    if (mesh && mesh->Alive()) {
        auto verts = mesh->GetVertices();
        mesh->SetVertices(verts);
        auto tris = mesh->GetTriangles();
        mesh->SetTriangles(tris);
        auto norms = mesh->GetNormals();
        mesh->SetNormals(norms);
        auto tangs = mesh->GetTangents();
        mesh->SetTangents(tangs);
        auto uvs = mesh->GetUV();
        mesh->SetUV(uvs);
        auto colors = mesh->GetColors();
        mesh->SetColors(colors);
        Bounds mb = mesh->GetBounds();
        mesh->SetBounds(mb);
        int subMeshCount = mesh->GetSubMeshCount();
        mesh->SetSubMeshCount(1);
        int vCount = mesh->GetVertexCount();
        mesh->RecalculateBounds();
        mesh->RecalculateNormals();
        mesh->RecalculateTangents();
        mesh->Clear(true);
        mesh->UploadMeshData(false);

        (void)verts; (void)tris; (void)norms; (void)tangs; (void)uvs; (void)colors;
        (void)mb; (void)subMeshCount; (void)vCount;
    }
}
#endif

#if defined(BNM_UNITY_AUDIO) || defined(BNM_UNITY_ANIMATION)
void TestUnityAudioAndAnimation() {
    using namespace BNM::UnityEngine;
    using namespace BNM::Structures::Unity;

#ifdef BNM_UNITY_AUDIO
    // 1. AudioClip
    AudioClip *clip = nullptr;
    if (clip && clip->Alive()) {
        float len = clip->GetLength();
        int samples = clip->GetSamples();
        int channels = clip->GetChannels();
        int freq = clip->GetFrequency();
        bool lData = clip->LoadAudioData();
        bool uData = clip->UnloadAudioData();
        int lState = clip->GetLoadState();

        (void)len; (void)samples; (void)channels; (void)freq; (void)lData; (void)uData; (void)lState;
    }

    // 2. AudioSource
    AudioSource *src = nullptr;
    if (src && src->Alive()) {
        auto c = src->GetClip();
        src->SetClip(clip);
        src->Play();
        src->Play(100);
        src->PlayDelayed(0.5f);
        src->PlayScheduled(1.0);
        src->PlayOneShot(clip, 1.0f);
        src->Pause();
        src->UnPause();
        src->Stop();
        bool isPlay = src->GetIsPlaying();
        bool isLoop = src->GetLoop();
        src->SetLoop(true);
        float aVol = src->GetVolume();
        src->SetVolume(1.0f);
        float aPitch = src->GetPitch();
        src->SetPitch(1.0f);
        float aTime = src->GetTime();
        src->SetTime(0.0f);
        float aSpatial = src->GetSpatialBlend();
        src->SetSpatialBlend(1.0f);
        bool aMute = src->GetMute();
        src->SetMute(false);
        float aMinD = src->GetMinDistance();
        src->SetMinDistance(1.0f);
        float aMaxD = src->GetMaxDistance();
        src->SetMaxDistance(500.0f);
        AudioSource::PlayClipAtPoint(clip, Vector3{0.f, 0.f, 0.f}, 1.0f);

        (void)c; (void)isPlay; (void)isLoop; (void)aVol; (void)aPitch; (void)aTime; (void)aSpatial; (void)aMute; (void)aMinD; (void)aMaxD;
    }

    // 3. AudioListener
    float gVol = AudioListener::GetVolume();
    AudioListener::SetVolume(1.0f);
    bool gPause = AudioListener::GetPause();
    AudioListener::SetPause(false);

    (void)gVol; (void)gPause;
#endif

#ifdef BNM_UNITY_ANIMATION
    // 4. AnimationClip & Animation
    AnimationClip *animClip = nullptr;
    if (animClip && animClip->Alive()) {
        float aLen = animClip->GetLength();
        float aFps = animClip->GetFrameRate();
        animClip->SetFrameRate(30.0f);
        int aWrap = animClip->GetWrapMode();
        animClip->SetWrapMode(1);
        bool aLoop = animClip->GetIsLooping();

        (void)aLen; (void)aFps; (void)aWrap; (void)aLoop;
    }

    Animation *anim = nullptr;
    if (anim && anim->Alive()) {
        anim->Play();
        anim->Play("Attack", 0);
        anim->CrossFade("Run", 0.3f, 0);
        anim->Blend("Walk", 0.5f, 0.2f);
        anim->Stop();
        anim->Stop("Attack");
        bool isA = anim->IsPlaying();
        bool isAttack = anim->IsPlaying("Attack");
        auto c = anim->GetClip("Attack");
        anim->AddClip(animClip, "CustomAnim");
        anim->RemoveClip("CustomAnim");
        anim->RemoveClip(animClip);
        int cCount = anim->GetClipCount();
        bool pAuto = anim->GetPlayAutomatically();
        anim->SetPlayAutomatically(true);
        int wMode = anim->GetWrapMode();
        anim->SetWrapMode(1);

        (void)isA; (void)isAttack; (void)c; (void)cCount; (void)pAuto; (void)wMode;
    }

    // 5. Animator
    Animator *animator = nullptr;
    if (animator && animator->Alive()) {
        int runHash = Animator::StringToHash("Run");
        animator->Play("Run", 0, 0.0f);
        animator->Play(runHash, 0, 0.0f);
        animator->CrossFade("Walk", 0.2f, 0, 0.0f);
        animator->CrossFade(runHash, 0.2f, 0, 0.0f);

        float speedVal = animator->GetFloat("Speed");
        animator->SetFloat("Speed", 1.0f);
        animator->SetFloat(runHash, 1.0f);
        bool isRunning = animator->GetBool("IsRunning");
        animator->SetBool("IsRunning", true);
        animator->SetBool(runHash, true);
        int stateIdx = animator->GetInteger("State");
        animator->SetInteger("State", 2);
        animator->SetInteger(runHash, 2);
        animator->SetTrigger("Jump");
        animator->SetTrigger(runHash);
        animator->ResetTrigger("Jump");
        animator->ResetTrigger(runHash);
        bool paramCurve = animator->IsParameterControlledByCurve("Speed");
        bool paramCurveHash = animator->IsParameterControlledByCurve(runHash);

        float animSpd = animator->GetSpeed();
        animator->SetSpeed(1.0f);
        bool rootMotion = animator->GetApplyRootMotion();
        animator->SetApplyRootMotion(true);
        int cullMode = animator->GetCullingMode();
        animator->SetCullingMode(0);
        int lCount = animator->GetLayerCount();
        auto lName = animator->GetLayerName(0);
        float lWeight = animator->GetLayerWeight(0);
        animator->SetLayerWeight(0, 1.0f);
        auto stateInfo = animator->GetCurrentAnimatorStateInfo(0);
        bool hasState = animator->GetHasState(0, runHash);
        animator->Rebind();
        animator->Update(0.016f);

        (void)runHash; (void)speedVal; (void)isRunning; (void)stateIdx; (void)paramCurve; (void)paramCurveHash;
        (void)animSpd; (void)rootMotion; (void)cullMode; (void)lCount; (void)lName; (void)lWeight;
        (void)stateInfo; (void)hasState;
    }
#endif
}
#endif


