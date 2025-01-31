using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Caching.Memory;
using System.Net.Http;

var builder = WebApplication.CreateBuilder(args);

builder.Services.AddHttpClient();
builder.Services.AddMemoryCache();
builder.Services.AddSingleton<IMDSv2TokenService>();
builder.Services.AddSingleton<IMDSv2CredentialService>();

var app = builder.Build();

app.MapGet("/iam-credentials", async (IMDSv2CredentialService credentialService) =>
{
    var credentials = await credentialService.GetIAMCredentialsAsync();
    return Results.Json(credentials);
});

app.Run();
